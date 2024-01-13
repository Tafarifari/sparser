#ifndef __SCRIPT_PARSER_H
	#define __SCRIPT_PARSER_H

	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <stdint.h>
	#include <stdbool.h>

	#ifndef __SUCCESS
		#define __SUCCESS 0
	#endif
	#ifndef __FAILURE
		#define __FAILURE -1
	#endif

	#define __HTML_TAG_HTML_OPEN	"<html>"
	#define __HTML_TAG_HTML_CLOS	"</html>"
	#define __HTML_TAG_PAR			"<p>"
	#define __HTML_TAG_ITLCS_ON		"<i>"
	#define __HTML_TAG_ITLCS_OFF	"</i>"
	#define __HTML_TAG_BOLD_ON		"<b>"
	#define __HTML_TAG_BOLD_OFF		"</b>"

typedef enum  {	
	PM_BOLD_BRACKETS = 	0x01,	// 0000 0001
	PM_ITAL_NARRATOR = 	0x02,	// 0000 0010
  	PM_HARD_ENDLINES = 	0x04, 	// 0000 0100
  /*PM_ = 				0x08,	// 0000 1000
	PM_ = 				0x10,	// 0001 0000
	PM_ =				0x20, 	// 0010 0000
	PM_ =				0x40,	// 0100 0000
	PM_	= 				0x80,	// 1000 0000*/
	PM_ALL =			0xFF	// 1111 1111
} ParsingMode;

typedef struct {
	char * timecode;
	char * speaker;
	char * line;
} ScriptLine;

typedef struct {
	ScriptLine * 	lines;
	int 			lines_num;
} ScriptFile;

extern void script_file_init(ScriptFile * sf); 									/* zero pointer and counter */

extern int script_file_read_file(ScriptFile * sf, const char * filename);		/* read script file parsing it into columns */
extern void script_file_parse_file(ScriptFile * sf, uint8_t mode);				/* inserts html tags into text, depending on mode */
extern void script_file_print_html(ScriptFile * sf, const char * separator);	/* prints file with html header to stdout */
extern void script_file_print_text(ScriptFile * sf, const char * separator, bool numbering);	/* prints file w/o html header, optional line numbering */

extern void script_file_erase(ScriptFile * sf);									/* deletes all data and frees allocated memory */

#endif