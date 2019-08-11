#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#define LINE_MAXSZ (1 << 16)
#define STANDARD_ONLY

#define STATIC_SIZE(arr) (sizeof (arr) / sizeof (arr)[0])
#define MK_STRLEN(str) {(str), STATIC_SIZE(str) - 1}


/* Standard mapping */
static inline const char* predomain_end(const char* predomain);
static inline const char* label_end(const char* label);
static inline const char* domain_end(const char* domain);
static inline const char* separator_end(const char* separator);
static inline bool map_standard(const char* line, const char** const result);


/* Returns whether the line was mapped */
/* result must point to a pre-allocated buffer */
typedef bool (*mapper_t)(const char* line, const char** const result);
mapper_t mappers[] = {
    map_standard,
};


static inline void remove_trailing_newline(char *text)
{
	int len;
	len = strlen(text);
	if (text[len-1] == '\n')
		text[len-1] = '\0';
}


typedef bool (*on_line_handler_t)(const char* line);
static void line_foreach(FILE* in_fd, on_line_handler_t handler)
{
    char linebuf[LINE_MAXSZ];

    while (fgets(linebuf, sizeof linebuf, in_fd)) {
        if (strlen(linebuf) == LINE_MAXSZ - 1) {
			fprintf(stderr, "WARNING: BUFFER LIMIT CAN EXCEED...\n");
			fprintf(stderr, "POSSIBLE UNDEFINED BEHAVIOR AHEAD!\n");
        }
        remove_trailing_newline(linebuf);
        if (!handler(linebuf))
            fprintf(stderr, "Failed to handle line:\n\t--> %s\n", linebuf);
    }
}


static bool map_line(const char* line)
{
    const char* pwd;
#ifdef STANDARD_ONLY
    if (map_standard(line, &pwd)) {
        fprintf(stdout, "%s\n", pwd);
        return true;
    }
#else /* STANDARD_ONLY */
    for (size_t i=0; i<STATIC_SIZE(mappers); ++i)
        if (mappers[i](line, &pwd)) {
            fprintf(stdout, "%s\n", pwd);
            return true;
        }
#endif /* STANDARD_ONLY */
    return false;
}


int main(int argc, char **argv)
{
	if (argc != 1) {
		printf("Usage: cat <filename> | %s\n",
		       argv[0]);
		return -1;
	}

    line_foreach(stdin, map_line);

	return 0;
}


static inline const char* predomain_end(const char* predomain)
{
    return strchr(predomain, '@');
}


static inline const char* label_end(const char* label)
{
    const char* ptr = label;

    if (label[0] == '-')
        return NULL;

    do {
        const char c = *ptr;
        if ('a' <= c && c <= 'z')
            continue;
        if ('A' <= c && c <= 'Z')
            continue;
        if ('0' <= c && c <= '9')
            continue;
        if (c == '-')
            continue;
        break;
    } while (*++ptr);

    return ptr[-1] == '-' ? NULL : ptr;
}


static inline const char* domain_end(const char* domain)
{
    const char* ptr = domain;

    while (1) {
        ptr = label_end(ptr);
        if (ptr == NULL)
            return NULL;
        if (ptr[0] == '.') {
            ++ptr;
            continue;
        }
        break;
    }

    return ptr;
}


static inline const char* separator_end(const char* separator)
{
    struct {
        const char* separator;
        int separator_len;
    } standard_separators[] = {
        /* Make sure no string follows its prefix */
        /* Most common prefix first for optimization */
        MK_STRLEN(":"),
        MK_STRLEN(";"),
        MK_STRLEN(" "),
        MK_STRLEN("|"),
        MK_STRLEN("\t"),
    };

    for (size_t i=0; i<STATIC_SIZE(standard_separators); ++i) {
        const char* sep = standard_separators[i].separator;
        const int len = standard_separators[i].separator_len;
        if (strncmp(separator, sep, len) == 0)
            return &separator[len];
    }

    return NULL;
}


/* Assumption: result does not contain the trailing newline */
static inline bool map_standard(const char* line, const char** const result)
{
    /* Assumption: line = xxx@<domain><separator><password><eol> */

    const char* at_pos = predomain_end(line);
    if (at_pos == NULL)
        return false;

    const char* email_end = domain_end(at_pos + 1);
    if (email_end == NULL)
        return false;

    const char* password_start = separator_end(email_end);
    if (password_start == NULL)
        return false;

    *result = password_start;
    return true;
}
