#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define LINE_MAXSZ 65536
#define PATH_MAXLEN 1024


int main(int argc, const char* argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: cat <input file> | %s <output directory> <number of partitions>\n", argv[0]);
        return -1;
    }

    char path[PATH_MAXLEN + 1];
    strncpy(path, argv[1], sizeof path);
    char* filename = &path[strlen(path)];
    if (filename[-1] != '/') {
        strcpy(filename, "/");
        ++filename;
    }
    const size_t filename_sz = &path[sizeof path] - filename;

    const unsigned n_sp = atoi(argv[2]);
    FILE** fds = malloc(n_sp * sizeof *fds);
    for (unsigned i = 0; i < n_sp; ++i) {
        snprintf(filename, filename_sz, "%u", i + 1);
        fds[i] = fopen(path, "w");
    }

    unsigned sp = 0;
    char in_buf[LINE_MAXSZ];
    while (fgets(in_buf, sizeof in_buf, stdin)) {
        fputs(in_buf, fds[sp]);
        sp = (sp + 1) % n_sp;
    }

    for (unsigned i = 0; i < n_sp; ++i)
        fclose(fds[i]);
    free(fds);
    return 0;
}
