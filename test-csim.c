#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <strings.h>
#define NUM_RESULTS 3
#define MAX_CHARS 6
#define FILENAME_MAXLEN 25

typedef struct {
    // number of set bits
    int s;
    // number of block bits
    int b;
    // number of lines per set
    int E;
    // file with memory references
    char *tracefile;

    int expHits;
    int expMisses;
    int expEvictions;
}TestCase;

int main(){
    char *resultsFile = ".cache_output";
    TestCase test_cases[] ={
        {.s = 2, .b = 3, .E = 5, .tracefile = "traces/traces0.trace", .expHits = 19, .expMisses = 11, .expEvictions = 1},
        {.s = 2, .b = 3, .E = 2, .tracefile = "traces/equals.trace", .expHits = 10, .expMisses = 1, .expEvictions = 0},
        {.s = 1, .b = 1, .E = 1, .tracefile = "traces/traces1.trace", .expHits = 9, .expMisses = 8, .expEvictions = 6},
        {.s = 4, .b = 4, .E = 2, .tracefile = "traces/traces2.trace", .expHits = 4, .expMisses = 5, .expEvictions = 2},
        {.s = 2, .b = 4, .E = 1, .tracefile = "traces/traces3.trace", .expHits = 2, .expMisses = 3, .expEvictions = 1}
    };

    // formats for args to ./cache_sim
    const char * const sFormat = "-s %d";
    const char * const bFormat = "-b %d";
    const char * const eFormat = "-E %d";
    const char * const tFormat = "-t%s";
    char sets[MAX_CHARS] = {0};
    char lines[MAX_CHARS] = {0};
    char bytes[MAX_CHARS] = {0};
    char tracefile[FILENAME_MAXLEN] = {0};
    size_t NUM_TESTS = sizeof(test_cases) / sizeof(test_cases[0]);
    for (int i = 0; i < NUM_TESTS; i++) {
        pid_t pid = fork();
        // populate args
        snprintf(sets, MAX_CHARS, sFormat, test_cases[i].s);
        snprintf(lines, MAX_CHARS, eFormat, test_cases[i].E);
        snprintf(bytes, MAX_CHARS, bFormat, test_cases[i].b);
        snprintf(tracefile, FILENAME_MAXLEN, tFormat, test_cases[i].tracefile);
        if (pid==0) {
    	    char* const cmd = "./cache_sim";
            execl(cmd, cmd, sets, lines, bytes, tracefile, NULL);
            exit(127); /* only if execv fails */
        }
        else if (pid > 0) {
            waitpid(pid,0,0);
            FILE *pFile = fopen(resultsFile, "r");
            if (!pFile) {
                perror("could not open file");
                exit(EXIT_FAILURE);
            }
            // compare to expected results
            int gotHits = -1;
            int gotMisses = -1;
            int gotEvictions = -1;
            int read = fscanf(pFile, "%d %d %d", &gotHits, &gotMisses, &gotEvictions);
            if (read != NUM_RESULTS) {
                printf("read %d, instead of %d numbers\n", read, NUM_RESULTS);
                return(-1);
            }
            int expHits = test_cases[i].expHits;
            int expMisses = test_cases[i].expMisses;
            int expEvictions = test_cases[i].expEvictions;
            if ((gotHits != expHits) || (gotMisses != expMisses) || (gotEvictions != expEvictions)) {
                printf(" - testcase %d: %s: [ FAIL ] : got/expected: hits: %d/%d, misses: %d/%d, evictions: %d/%d\n", i, test_cases[i].tracefile, gotHits, expHits, gotMisses, expMisses, gotEvictions, expEvictions);
            }else printf(" - testcase %d: %s: [ PASS ]\n", i, test_cases[i].tracefile);
            fclose(pFile);
        }
        else {
            perror("could not create child process");
        }
    }
    return 0;
}
