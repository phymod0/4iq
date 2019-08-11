#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#define LINE_MAXSZ 65536


int main(void)
{
    size_t count = 0;
    bool first_iteration = true;
    char prev_buf[LINE_MAXSZ];
    char buf[LINE_MAXSZ];

    while (fgets(buf, sizeof buf, stdin)) {
        if (first_iteration) {
            memcpy(prev_buf, buf, sizeof prev_buf);
            ++count;
            first_iteration = false;
        } else if (strcmp(prev_buf, buf) == 0) {
            ++count;
        } else {
            printf("%lu %s", count, prev_buf);
            memcpy(prev_buf, buf, sizeof prev_buf);
            count = 1;
        }
    }
    printf("%lu %s", count, prev_buf);

    return 0;
}
