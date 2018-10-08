#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static inline int is_domain_char(unsigned char c)
{
	int i, n_allowed_specials;
	char allowed_specials[] = ".-";

	n_allowed_specials = sizeof allowed_specials;
	if ('a' <= c && c <= 'z')
		return 1;
	if ('A' <= c && c <= 'Z')
		return 1;
	if ('0' <= c && c <= '9')
		return 1;
	for (i=0; i<n_allowed_specials; i++)
		if (allowed_specials[i] == c)
			return 1;

	return 0;
}


static int is_bad_delim(char *text)
{
	int i, n_delims;
	char *delims[] = {
		"{newline}",
		"yahoo!",
		"yahoo.com!",
	};

	n_delims = sizeof delims / sizeof delims[0];
	for (i=0; i<n_delims; i++) {
		int delim_len = strlen(delims[i]);
		if (!strncmp(text, delims[i], delim_len))
			return delim_len;
	}

	return 0;
}


static unsigned char get_delim(char *str)
{
	while (*str) {
		if (*str == '@')
			break;
		++str;
	}
	if (*str != '@') {
		fprintf(stderr, "WARNING: INVALID EMAIL...\n");
		return 0;
	}
	++str;
	while (*str) {
		int bad_delim_len;
		if ((bad_delim_len = is_bad_delim(str))) {
			str += bad_delim_len;
			continue;
		}
#if WARN_NO_ENTRY_PWD
		if (*str == '\n')
			fprintf(stderr, "WARNING: NO PASSWORD"
				" FOR THIS ENTRY...\n");
#endif /* WARN_NO_ENTRY_PWD */
		if (!is_domain_char(*str))
			return *str;

		++str;
	}

	fprintf(stderr, "WARNING: INVALID ENTRY...\n");
	return 0;
}


int main(int argc, char **argv)
{
	char linebuf[65536];
	int i, chars[256] = {0};

	if (argc != 1) {
		printf("Usage: cat <filename> | %s\n",
		       argv[0]);
		return -1;
	}

	while (fgets(linebuf, sizeof linebuf, stdin)) {
		if (strlen(linebuf) > 60000)
			fprintf(stderr, "WARNING: BUFFER LIMIT"
				" CAN EXCEED...\n");
		chars[get_delim(linebuf)] += 1;
	}

	printf("Characters appearing:\n");
	for (i = 0; i < 256; i++)
		if (chars[i])
			printf("\t%u[%c] (%d times)\n", i, i,
			       chars[i]);

	return 0;
}
