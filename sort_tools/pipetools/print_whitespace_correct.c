#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static void remove_trailing_newline(char *text)
{
	int len;
	len = strlen(text);
	if (text[len-1] == '\n')
		text[len-1] = '\0';
	return;
}


static int is_bad_char(char c)
{
	return (c == '\t') || (c == ' ');
}


static char* corrected_line(char *line)
{
	char *old_line;
	remove_trailing_newline(line);
	old_line = line;
	while (is_bad_char(line[0]))
		++line;
	if (line[0] == '\0')
		/*
		 * The password was probably supposed to be a bunch of
		 * whitespace characters. Return as-is.
		 */
		return old_line;
	return line;
}


static int is_bad_line(char *line, char **corrected)
{
	*corrected = corrected_line(line);
	// return line != *corrected;
    return 0;
}


static void correct_bad_lines(void)
{
	int count = 0;
	char line_buf[65536], *corrected;

	while (fgets(line_buf, sizeof line_buf, stdin)) {
		if (!is_bad_line(line_buf, &corrected)) {
#if 0
			printf("Corrected line \"%s\" to \"%s\"\n",
			       line_buf, corrected);
#endif
			printf("%s\n", corrected);
		}
	}

	return;
}


int main(int argc, char *argv[])
{
	if (argc != 1) {
		printf("Usage: cat <filename> | %s\n", argv[0]);
		return -1;
	}

	correct_bad_lines();

	return 0;
}
