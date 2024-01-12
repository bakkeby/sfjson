/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX(A,B) ((A) > (B) ? (A) : (B))

void
usage(void)
{
	fprintf(stderr,
		"Usage: sfjson                                                                 \n"
		"                                                                              \n"
		"Reads JSON data from stdin, formats and indents it and writes the result to   \n"
		"stdout.                                                                       \n"
		"                                                                              \n"
		"Contrary to generally recommended tools like jq or python -mjson.tool this    \n"
		"program does not perform any form of validation to ensure that the input      \n"
		"is valid JSON. Formatting is done on a best effort basis following basic      \n"
		"rules.                                                                        \n"
		"                                                                              \n"
		"The benefit of this is that the output will be consistent even for incomplete \n"
		"data and the tool will not refuse to do the job if it doesn't understand it.  \n"
		"                                                                              \n"
		"This is intended as simple tool to make condensed (e.g. single line) JSON like\n"
		"data structures more readable by adding line breaks and indentation after     \n"
		"square brackets, curly braces and commas while ignoring escaped characters as \n"
		"well as leaving text encapsulated in double or single quotes as-is.           \n"
		"                                                                              \n"
		"Example usage:                                                                \n"
		"   $ sfjson < file.json                                                       \n"
		"   $ echo '{\"key\":\"99\",\"id\":3,\"opts\":{\"yes\":1,\"no\":0}}' | sfjson  \n"
		"\n"
	);
	exit(0);
}

void
printindent(int level)
{
	int l;
	char indent = 0x09; // tab character
	for (l = 0; l < level; ++l) {
		putchar(indent);
	}
}

void
format_json(void)
{
	int i;
	int level = 0;
	int escape_next = 0;
	int inside_double_quote = 0;
	int inside_single_quote = 0;
	int indent_if_not_square_closing_bracket = 0;
	int indent_if_not_curly_closing_bracket = 0;
	int skip_next_spaces = 1;
	int buffer_size = 100;
	char buffer[buffer_size];

	while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
		for (i = 0; i < buffer_size; i++) {
			if (buffer[i] == 0) {
				break;
			}

			if (escape_next) {
				escape_next = 0;
				goto next;
			}

			if (skip_next_spaces) {
				if (buffer[i] == 0x20 || buffer[i] == 0x09) {
					continue;
				}
				skip_next_spaces = 0;
			}

			if (buffer[i] == '\\') {
				escape_next = 1;
				goto next;
			}

			if (inside_double_quote && buffer[i] != '"') {
				goto next;
			}

			if (inside_single_quote && buffer[i] != '\'') {
				goto next;
			}

			if (indent_if_not_square_closing_bracket) {
				indent_if_not_square_closing_bracket = 0;
				if (buffer[i] == ']') {
					goto next;
				}
				putchar('\n');
				printindent(++level);
			}

			if (indent_if_not_curly_closing_bracket) {
				indent_if_not_curly_closing_bracket = 0;
				if (buffer[i] == '}') {
					goto next;
				}
				putchar('\n');
				printindent(++level);
			}

			if (buffer[i] == '\n') {
				skip_next_spaces = 1;
				continue;
			}

			if (buffer[i] == '"') {
				if (!inside_single_quote) {
					inside_double_quote = !inside_double_quote;
				}
				goto next;
			}

			if (buffer[i] == '\'') {
				if (!inside_double_quote) {
					inside_single_quote = !inside_single_quote;
				}
				goto next;
			}

			if (buffer[i] == '[') {
				indent_if_not_square_closing_bracket = 1;
				goto next;
			}

			if (buffer[i] == '{') {
				indent_if_not_curly_closing_bracket = 1;
				goto next;
			}

			if (buffer[i] == '}' || buffer[i] == ']') {
				putchar('\n');
				level = MAX(level - 1, 0);
				printindent(level);
				goto next;
			}

			if (buffer[i] == ',') {
				putchar(buffer[i]);
				putchar('\n');
				printindent(level);
				skip_next_spaces = 1;
				continue;
			}
next:
			putchar(buffer[i]);
 		}
	}
}

int
main(int argc, char *argv[])
{
	if (isatty(STDIN_FILENO)) {
		usage();
	}

	format_json();
	exit(0);
}
