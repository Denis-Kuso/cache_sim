#include "cachelab.h"
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <stdbool.h>
#include "init_cache.c"

int main(int argc, char *argv[])
{
//usage: `./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>`
    int opt;
    uint16_t setBits, lines, bytes;
    char *tracefile;
    bool verbose;

    setBits = 1;
    lines = 1;
    bytes = 1;
    verbose = false;
    tracefile = "";

    while ((opt = getopt(argc, argv, "hvs:E:b:t:" )) != -1 )
    {
        switch (opt) {
            case 'h':
                fprintf(stdout, "Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n",argv[1]);
                exit(EXIT_SUCCESS);
            case 'v':
                verbose = true;
                break;
            case 's':
                setBits = atoi(optarg);
                break;
            case 'E': 
                lines = atoi(optarg);
                break;
            case 'b':
                bytes = atoi(optarg);
                break;
            case 't':
                tracefile = optarg;
                break;
            default: /* '?' */
                fprintf(stderr, "Usage: %s [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n",argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (verbose) {
        printf("verbose enabled\n");
    }
    fprintf(stdout, "Args parsed: -s %d -E %d -b %d -t %s\n", setBits, lines, bytes, tracefile);
    FILE * pFile;
    pFile = fopen(tracefile, "r");
    if (!pFile) {
        fprintf(stderr, "unable to open %s\n", tracefile);
        exit(EXIT_FAILURE);
    }
    char operation_id;
    unsigned long long address;
    int size;
    // Line is of format " M 0x3242,4" or "I 0x324234,4"
    const char *format = " %c %llx,%d";
    int counter = 0;
    cache_line **cache = init_cache(setBits, lines);
    if (!cache) {
        return EXIT_FAILURE;
    }
    int hits, misses = 0, evictions = 0;
    int const sets = 1 << setBits;
    int iters;
    int const OPERATIONS = 2;
    while (fscanf(pFile, format, &operation_id, &address, &size) > 0) {
        printf("Parsed line: #%d. Operation: %c, address: %10llx, size: %d\n", counter, operation_id, address, size);
        iters = 0;
        if (operation_id != 'M') {
            printf("will do iter once\n");
            iters++;
        }
        // modify is a read and a write (so two access operations)
        while (iters < OPERATIONS) {

        // increment time on every entry *
        for (int i = 0; i < sets; i++) {
            for (int j = 0;j < lines; j++) {
                cache[i][j].LRU += 1;
                }
        }
        bool cached = false;
      // extract sID
        int setID = extractSetBits(address, bytes, setBits);
        // extract Tag
        int tag = extractTagBits(address, bytes, setBits);
        // is address cached (M requires two operations)
        for (int j = 0; j < lines; j++) {
            cached = (cache[setID][j].valid && (cache[setID][j].tag == tag));
            if (cached) {
                hits++; // dont forget to update LRU
                cache[setID][j].LRU = 0;
                break;
            }
        }
        if (!cached) {
            misses++; 
            bool full = true;
            int max = -1;
            int max_index = 0;
            for (int j = 0; j < lines; j++) {
                // perhaps store index of max LRU in case eviction is neccessary
                if (cache[setID][j].LRU > max) {
                    max_index = j;
                }
                // find empty
                if (!cache[setID][j].valid) {
                    cache[setID][j].valid = true;
                    cache[setID][j].LRU = 0;
                    cache[setID][j].tag = tag;
                    full = false;
                    break;
                }
            }
            // store somewhere, update LRU
            if (full) {
                evictions++;
                // evict at max_index
                cache[setID][max_index].valid = true; // redundant?
                cache[setID][max_index].LRU = 0;
                cache[setID][max_index].tag = tag;
            }
        }
            iters++;
        }
        counter++;
    }
    fclose(pFile);
    printSummary(hits, misses, evictions);
    return 0;
}
