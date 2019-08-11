#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


static inline bool is_num(char c)
{
    return '0' <= c && c <= '9';
}


static inline const char* map(const char* line)
{
    const char* ptr = line;
    char c;

    while ((c = *ptr)) {
        if (is_num(c)) {
            const char* space = strchr(ptr, ' ');
            return space ? space + 1 : NULL;
        }
        ++ptr;
    }

    return NULL;
}


int main(void)
{
    char buf[65536];
    while (fgets(buf, sizeof buf, stdin)) {
        const char* mapped = map(buf);
        if (mapped)
            fprintf(stdout, "%s", mapped);
        else
            fprintf(stderr, "ERROR: Could not process line: %s", buf);
    }

    return 0;
}
