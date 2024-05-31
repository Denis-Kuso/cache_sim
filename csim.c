#include "cachelab.h"
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <stdbool.h>
//#include "init_cache.c"

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
    int i = 0;
    while (fscanf(pFile, format, &operation_id, &address, &size) > 0) {
        printf("Parsed line: #%d. Operation: %c, address: %10llx, size: %d\n", i, operation_id, address, size);
        i++;
    }
    fclose(pFile);
    printSummary(setBits, lines, bytes);
    return 0;
}
