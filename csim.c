#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <stdbool.h>
#include "init_cache.c"

void printInfo(int hits, int misses, int evictions);

int main(int argc, char *argv[])
{
    char * const msg = "Usage: ./cache_sim [-hv] -s <s> -E <E> -b <b> -t <tracefile>";
    int opt;
    uint16_t setBits, lines, bytes;
    char *tracefile;
    bool verbose;

    setBits = 1;
    lines = 1;
    bytes = 1;
    verbose = false;

    while ((opt = getopt(argc, argv, "hvs:E:b:t:" )) != -1 )
    {
        switch (opt) {
            case 'h':
                fprintf(stdout, "%s\n", msg);
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
                fprintf(stderr, "%s\n", msg);
                exit(EXIT_FAILURE);
        }
    }
    FILE * pFile;
    pFile = fopen(tracefile, "r");
    if (!pFile) {
        perror("error");
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
    int hits = 0, misses = 0, evictions = 0;
    int const sets = 1 << setBits;
    int iters;
    int const OPERATIONS = 2;
    char result[20];
    while (fscanf(pFile, format, &operation_id, &address, &size) > 0) {
        memset(result,0,strlen(result));
        // not interested
        if (operation_id == 'I') {
            continue;
        }
        iters = 0;
        if (operation_id != 'M') {
            iters++;
        }
        // modify is a read and a write (so two access operations)
        while (iters < OPERATIONS) {

        // increment time on every lookup 
        for (int i = 0; i < sets; i++) {
            for (int j = 0;j < lines; j++) {
                cache[i][j].LRU += 1;
            }
        }
        bool cached = false;
        int setID = extractSetBits(address, bytes, setBits);
        int tag = extractTagBits(address, bytes, setBits);
        // is address cached
        for (int j = 0; j < lines; j++) {
            cached = (cache[setID][j].valid && (cache[setID][j].tag == tag));
            if (cached) {
                strcat(result, "hit ");
                hits++;
                cache[setID][j].LRU = 0;
                break;
            }
        }
        if (!cached) {
            strcat(result, "miss ");
            misses++; 
            bool full = true;
            int max = -1;
            int max_index = 0;
            for (int j = 0; j < lines; j++) {
                // store index of max LRU in case eviction is neccessary
                if (cache[setID][j].LRU > max) {
                    max = cache[setID][j].LRU;
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
            // store at max_index, update LRU
            if (full) {
                strcat(result, "eviction ");
                evictions++;
                // evict at max_index
                cache[setID][max_index].valid = true;
                cache[setID][max_index].LRU = 0;
                cache[setID][max_index].tag = tag;
            }
        }
        iters++;
        }
        if (verbose) {
            fprintf(stdout, "%c %11llx, %d %s\n",operation_id, address, size, result);
        }
        counter++;
    }
    fclose(pFile);
    printInfo(hits, misses, evictions);
    return 0;
}

void printInfo(int hits, int misses, int evictions) {
    printf("hits: %d, misses: %d, evictions: %d\n", hits, misses, evictions);
    FILE* results = fopen(".cache_output", "w");
    if (!results) {
        perror("ERR:");
        exit(EXIT_FAILURE);
    }
    fprintf(results, "%d %d %d", hits, misses, evictions);
    fclose(results);
}
