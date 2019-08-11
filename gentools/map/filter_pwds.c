#include <stdio.h>
#include <string.h>
#include <stdlib.h>


static int is_domain_char(unsigned char c)
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


static void remove_trailing_newline(char *text)
{
	int len;
	len = strlen(text);
	if (text[len-1] == '\n')
		text[len-1] = '\0';
	return;
}


static inline int printable(unsigned char c)
{
	return 32 <= c && c <= 126;
}


static inline int has_nonprintable(char *text)
{
	for (; *text; ++text)
	if (!printable(*text))
		return 1;
	return 0;
}


static inline void write_if_printable(char *text, FILE *fd)
{
	if (has_nonprintable(text))
		return;
	fprintf(fd, "%s\n", text);
}


static inline void write_text(char *text, FILE *fd)
{
	remove_trailing_newline(text);
	if (strlen(text) > 0)
		write_if_printable(text, fd);
	return;
}


static void write_pwd(char *pwd, char delim)
{
	char *trailing_pwd;

	switch (delim) {
	case ';':
		/*
		 * The actual delimiter could be the full colon
		 * that may follow, write that too...
		 */
		trailing_pwd = strchr(pwd, ':');
		if (trailing_pwd)
			write_text(trailing_pwd+1, stdout);
		/* fall-thru */
	case '\t':
	case ' ':
	case ':':
	case '|':
		write_text(pwd, stdout);
		/* fall-thru */
	default:
		/* Weird entry or encountered a newline */
		break;
	}

	return;
}


static void write_pwd_from(char *str)
{
	while (*str) {
		if (*str == '@')
			break;
		++str;
	}
	if (*str != '@') {
		fprintf(stderr, "WARNING: INVALID EMAIL...\n");
		return;
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
		if (!is_domain_char(*str)) {
			char bad_delim;
			bad_delim = *str;
			write_pwd(str+1, bad_delim);
			return;
		}

		++str;
	}

	fprintf(stderr, "WARNING: INVALID ENTRY...\n");
	return;
}


int main(int argc, char **argv)
{
	char linebuf[65536];

	if (argc != 1) {
		printf("Usage: cat <filename> | %s\n",
		       argv[0]);
		return -1;
	}

	while (fgets(linebuf, sizeof linebuf, stdin)) {
		if (strlen(linebuf) > 60000)
			fprintf(stderr, "WARNING: BUFFER LIMIT"
				" CAN EXCEED...\n");
		write_pwd_from(linebuf);
	}

	return 0;
}
