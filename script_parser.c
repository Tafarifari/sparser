#include <assert.h>
#include "script_parser.h"
#include "read_string.h"
#include "err_handler.h"

//#include "debug_mem.h"

const char * html_on 	= __HTML_TAG_HTML_OPEN;
const char * html_off 	= __HTML_TAG_HTML_CLOS;
const char * par 		= __HTML_TAG_PAR;	
const char * itl_on 	= __HTML_TAG_ITLCS_ON;
const char * itl_off 	= __HTML_TAG_ITLCS_OFF;
const char * bold_on 	= __HTML_TAG_BOLD_ON;
const char * bold_off 	= __HTML_TAG_BOLD_OFF;

/*const char * html_on 	= "<html>";
const char * html_off 	= "</html>";
const char * par 		= "<p>";	
const char * itl_on 	= "<i>";
const char * itl_off 	= "</i>";
const char * bold_on 	= "<b>";
const char * bold_off 	= "</b>";*/

void script_file_init(ScriptFile * sf)
{
	sf->lines = NULL;
	sf->lines_num = 0;
}


int script_file_add_line(ScriptFile * sf)
{
	if(sf->lines == NULL) {
		sf->lines = malloc(sizeof(ScriptLine));
		sf->lines_num = 1;
	} else {
		++sf->lines_num;
		size_t new_size = sf->lines_num * sizeof(ScriptLine);
		sf->lines = realloc(sf->lines, new_size);
	}

	if(sf->lines == NULL) {
		fprintf(stderr, "script_file_add_line ERROR: malloc()/realloc() failed\n");
		exit(1);
	}	

	return sf->lines_num;
}


int script_file_read_file(ScriptFile * sf, const char * filename)
{
	FILE * fp = fopen(filename, "r");
	if(fp == NULL) {
		fprintf(stderr, "ScriptFile ERROR: can't open file %s\n", filename);
		return __FAILURE;
	}

	int line_count = 0;

	char * string;
	while((string = read_string(fp)) != NULL) 
	{
		long len = strlen(string);
		int cols = 0;
		char * col_1 = string;
		char * col_2 = NULL;
		char * col_3 = NULL;

		int i;
		for(i = 0; i < len; i++)
		{
			if(string[i] == '\t') {
				if(cols == 0) {
					col_2 = &string[i+1];
					cols++;
				} else if(cols == 1) {
					col_3 = &string[i+1];
					cols++;
				} else {
					--i;
					break;
				}
			}
		}

		size_t col_1_len = col_2 - col_1; // includes \t (replaced by \0)
		size_t col_2_len = col_3 - col_2; // includes \t (replaced by \0)
		size_t col_3_len = i - col_2_len - col_1_len + 1;  // no \t, 1 extra byte for \0

		int line_idx = script_file_add_line(sf) - 1;
		CHECK_ALLOC(sf->lines[line_idx].timecode = malloc(col_1_len));
		CHECK_ALLOC(sf->lines[line_idx].speaker = malloc(col_2_len));
		CHECK_ALLOC(sf->lines[line_idx].line = malloc(col_3_len));

		memcpy(sf->lines[line_idx].timecode, col_1, col_1_len);
		sf->lines[line_idx].timecode[col_1_len-1] = '\0';

		memcpy(sf->lines[line_idx].speaker, col_2, col_2_len);
		sf->lines[line_idx].speaker[col_2_len-1] = '\0';

		memcpy(sf->lines[line_idx].line, col_3, col_3_len);
		sf->lines[line_idx].line[col_3_len-1] = '\0';

		++line_count;
		free(string);
	}

	fclose(fp);
	return line_count;
}


#define CONTAINS(PAT) (((mode & PAT) == PAT) ? true : false)

void script_file_parse_file(ScriptFile * sf, uint8_t mode)
{
	bool flag_narrator = false;

	for(int i=0; i<sf->lines_num; ++i) 
	{
		if(CONTAINS(PM_BOLD_BRACKETS))
		{
			uint16_t * ptrs = NULL;
			uint ptrs_num = 0;
			const size_t old_len = strlen(sf->lines[i].line);

			// search for bolded phrases
			bool flag_open_sqrbr = false;
			bool flag_closed_sqrbr = false;
			bool flag_text_sequence = false;
			for(int j=0; j<old_len; ++j)
			{
				uint16_t sqbr_opn;
				uint16_t sqbr_cls;

				if(sf->lines[i].line[j] == '[') {
					if(flag_open_sqrbr) {
						fprintf(stderr, "script_file_parse_file ERROR: unexpected \"[\" at line %d:%d\n", i, j);
						continue;
					} else {
						sqbr_opn = j;
						flag_open_sqrbr = true;
						flag_text_sequence = true;
					}
				} 
				else if(sf->lines[i].line[j] == ']') {
					if(!flag_open_sqrbr) {
						fprintf(stderr, "script_file_parse_file ERROR: unexpected \"]\" at line %d:%d\n", i, j);
						continue;
					} else {
						sqbr_cls = j + 1; // point to NEXT character after sqr bracket
						flag_open_sqrbr = false;
						flag_closed_sqrbr = true;
					}
				}

				// skip bracketed timecodes
				if(flag_open_sqrbr) {
					if(sf->lines[i].line[j] >= '0' && sf->lines[i].line[j] <= '9') flag_text_sequence = false;
				}

				// save indexes
				if(flag_closed_sqrbr) {
					if(flag_text_sequence) {
						ptrs_num += 2;
						CHECK_ALLOC(ptrs = realloc(ptrs, ptrs_num * sizeof(uint16_t)));
						ptrs[ptrs_num-2] = sqbr_opn;
						ptrs[ptrs_num-1] = sqbr_cls;
					}
					flag_closed_sqrbr = false;
					flag_text_sequence = false;
				}
			}
			// end of for-loop
			// whole line scanned

			// add formatting
			if(ptrs_num) 
			{
				// count new length
				size_t added_len = 0;
				for(int j=0; j<ptrs_num; j+=2) {
					added_len += (ptrs[j+1] - ptrs[j]) + strlen(bold_on) + strlen(bold_off);
				}

				size_t new_len = old_len + added_len + 1; // byte for trailin \0
				char * new_line = NULL;
				CHECK_ALLOC(new_line = malloc(new_len));

				size_t idx = 0;
				
				// copy text before the first square bracket
				if(ptrs[0] > 0) {
					memcpy(new_line, sf->lines[i].line, ptrs[0]);
					idx += ptrs[0];
				}

				for(int j=0; j<ptrs_num; j+=2) 
				{
					// <b>
					memcpy(&new_line[idx], bold_on, strlen(bold_on));
					idx += strlen(bold_on);
					// [...]
					memcpy(&new_line[idx], &(sf->lines[i].line[ptrs[j]]), (ptrs[j+1] - ptrs[j]));
					idx += (ptrs[j+1] - ptrs[j]);
					// </b>
					memcpy(&new_line[idx], bold_off, strlen(bold_off));
					idx += strlen(bold_off);
					
					// no more bracket squares, copy the rest and null-terminate string
					if(ptrs_num == j + 2) 
					{
						memcpy(&new_line[idx], &(sf->lines[i].line[ptrs[j+1]]), old_len - ptrs[j+1]);
						idx += (old_len - ptrs[j+1]);
						new_line[idx] = '\0';
					} 
					// more bracket squares, copy text in the middle
					else
					{
						memcpy(&new_line[idx], &(sf->lines[i].line[ptrs[j+1]]), (ptrs[j+2] - ptrs[j+1]));
						idx += (ptrs[j+2] - ptrs[j+1]);
					}
				}
			
				free(sf->lines[i].line);
				sf->lines[i].line = new_line;
				free(ptrs);
			}	
		}

		if(CONTAINS(PM_ITAL_NARRATOR))
		{
			if(flag_narrator) {
				if(strlen(sf->lines[i].speaker) > 0 && strcmp(sf->lines[i].speaker, "NARR") != 0)
					flag_narrator = false;
			} else {
				if(strcmp(sf->lines[i].speaker, "NARR") == 0)
					flag_narrator = true;
			}
			
			if(flag_narrator) 
			{
				// add <i>
				size_t old_len = strlen(sf->lines[i].timecode);
				size_t new_len = old_len + strlen(itl_on) + 1; // 1 byte for trailing \0
				
				char * ptr;
				CHECK_ALLOC(ptr = malloc(new_len));
				memcpy(ptr, itl_on, strlen(itl_on));
				memcpy(&ptr[strlen(itl_on)], sf->lines[i].timecode, old_len);
				ptr[new_len-1] = '\0';
				free(sf->lines[i].timecode);
				sf->lines[i].timecode = ptr;
				ptr = NULL;

				// add </i>
				old_len = strlen(sf->lines[i].line);
				new_len = old_len + strlen(itl_off) + 1; // 1 byte for trailing \0

				CHECK_ALLOC(sf->lines[i].line = realloc(sf->lines[i].line, new_len));
				memcpy(&(sf->lines[i].line[old_len]), itl_off, strlen(itl_off));
				sf->lines[i].line[new_len-1] = '\0';
			}
		}	

		if(CONTAINS(PM_HARD_ENDLINES)) 
		{
			assert(sf->lines[i].line != NULL);

			size_t old_len = strlen(sf->lines[i].line);
			size_t new_len = old_len + strlen(par) + 1; // 1 byte for trailing \0
			
			char * temp = realloc(sf->lines[i].line, new_len);
			if(temp == NULL) {
				fprintf(stderr, "script_file_parse_file ERROR (line %d): realloc() failed\n", i);
				exit(-1);
			} else {
				sf->lines[i].line = temp;
			}

			memcpy(&sf->lines[i].line[old_len], par, strlen(par));
			sf->lines[i].line[new_len-1] = '\0';
		}	
	}
}



void script_file_print_html(ScriptFile * sf, const char * separator)
{
	printf("%s\n", html_on);
	for(int i=0; i<sf->lines_num; ++i) {
		printf("%s%s%s%s%s\n",
				sf->lines[i].timecode,
				separator,
				sf->lines[i].speaker,
				separator,
				sf->lines[i].line);
	}
	printf("%s\n", html_off);
}

void script_file_print_text(ScriptFile * sf, const char * separator, bool numbering)
{
	for(int i=0; i<sf->lines_num; ++i) 
	{
		if(numbering) printf("%04d ", i);
		printf("%s%s%s%s%s\n",
				sf->lines[i].timecode,
				separator,
				sf->lines[i].speaker,
				separator,
				sf->lines[i].line);
	}
}

void script_file_erase(ScriptFile * sf)
{
	for(int i=0; i<sf->lines_num; ++i) 
	{
		free(sf->lines[i].timecode);
		free(sf->lines[i].speaker);
		free(sf->lines[i].line);
	}
	free(sf->lines);
	sf->lines = NULL;
	sf->lines_num = 0;
}