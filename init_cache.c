#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "parse-address.c"

// Tag and valid bit used for retrieval of cached entry
// LRU represent number of cache lookups since last use
typedef struct{
    /*
    * Identification
    */
    int tag;
    /*
    * Ticks since last used
    */
    int LRU;
    /*
    * true indicates valid entry
    */
    bool valid;
}cache_line;

cache_line **init_cache(int setBits, int lines){

    int sets = 1 << setBits;
    cache_line **cache;
    cache = malloc(sets * sizeof(int *));
    if (!cache) {
        fprintf(stderr, "No memory! Cannot allocate cache sets\n");
        return NULL;
    }
    for (int i = 0; i < sets; i++) {
        cache[i] = (cache_line*)malloc(lines * sizeof(cache_line));
        if (!cache[i]) {
            fprintf(stderr, "No memory! Cannot allocate cache lines\n");
            return NULL;
        }
    }
    // init empty cache
    for (int r = 0; r < sets; r++) {
        for (int n = 0; n < lines; n++) {
            cache[r][n].valid = false;
            cache[r][n].tag = 0;
            cache[r][n].LRU = 0;
        }
    }
    return cache;
}

int free_cache(cache_line **cache, int setBits, int lines) {
    int sets = 1 << setBits;
    for (int i = 0; i < sets; i++) {
        free(cache[i]);
    }
    free(cache);
    return 0;
}
