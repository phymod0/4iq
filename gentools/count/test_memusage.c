#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "trie/src/trie.h"


#define PWD_MAXLEN 65536


size_t get_int_size(void* v)
{
    (void)v;

    return sizeof(int);
}


int main(void)
{
    Trie* t = trie_create(trie_makeops(free, get_int_size));

    char buf[PWD_MAXLEN];
    while (fgets(buf, sizeof buf, stdin)) {
        int* val = trie_find(t, buf);
        if (val) {
            ++*val;
        } else {
            val = malloc(sizeof *val);
            *val = 0;
            trie_insert(t, buf, val);
        }
    }

    printf("Got trie mem usage: %lu\n", trie_memory_usage(t));

    trie_destroy(t);
    return 0;
}
