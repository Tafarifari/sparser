#include <stdio.h>
#include <stdlib.h>
#include "script_parser.h"

//#include "debug_mem.h"

int main(int argc, char ** argv)
{
	ScriptFile script;

	if(argc < 2) {
		printf("USAGE: %s <filename>\n", argv[0]);
		exit(0);
	}

	script_file_init(&script);

	script_file_read_file(&script, argv[1]);
	script_file_parse_file(&script, PM_HARD_ENDLINES | PM_ITAL_NARRATOR | PM_BOLD_BRACKETS);
	script_file_print_html(&script, "|--TAB--|");
	//script_file_print_text(&script, "\t", false);

	script_file_erase(&script);

	return 0;
}