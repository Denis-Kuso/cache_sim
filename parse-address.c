#include <stdint.h>
#include <stdio.h>

// for testing purposes
#define NUM_ADRS 5
#define NUM_COMB 4

/*
* Given a memory address, number of bit positions (bits)
* allocated for data, number of bit positions (sets) for 
* a set index, determine what the tag is
*/ 
int extractTagBits(uint64_t address, uint64_t bits, uint64_t sets) {
    uint64_t const ARCH = 64;
    uint64_t tagID = 0;
    uint64_t mask = 0x1;
    uint64_t value = address >> (bits + sets);
    uint64_t positions = ARCH - (bits + sets);
    for (int i = 0; i < positions; i++){
        tagID += mask & value;
        mask = mask << 1;
    }
    return tagID;
}

/*
* Given a memory address, number of bit positions (bits)
* allocated for data, number of bit positions (sets) for 
* a set index, determine what the set index is
*/ 
int extractSetBits(uint64_t address, uint64_t bits, uint64_t sets) {
    int mask = 0x1;
    int setID = 0;
    int value = address >> bits;
    for (int i = 0; i < sets; i++){
        setID = setID + (mask & value);
        mask = mask << 1;
    }
    return setID;
}


void testExtractSet() {
    int adrs[4]={0, 0x10,0x16,0x64};
    int bits[4] = {1,4, 2, 4};
    int sets[4] = {1, 2, 3, 4};
    int exp[4][4] = {{0, 0, 0, 0}, {0, 1, 4, 1}, {1, 1, 5, 1},{0, 2, 1, 6}};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j<4;j++) {
            int got = extractSetBits(adrs[i], bits[j], sets[j]);
            int want = exp[i][j]; 
            if (want != got) {
                printf("- [ FAIL ]:\n");
                printf("  Address: %x, sets: %d, bits: %d, got setID: %d\n", adrs[i], sets[j], bits[j], got);
            }else printf("- [ PASS ]\n");
        }
    }
}


void testExtractTag() {
    uint64_t adrs[NUM_ADRS] = {0, 0x10, 0x16, 0x64, 0x7ff000384};
    int bits[4] = {1, 4, 2, 4};
    int sets[4] = {1, 2, 3, 4};
    uint64_t exp[NUM_ADRS][NUM_COMB] = {{0, 0, 0, 0}, {4, 0, 0, 0}, {5, 0, 0, 0}, {25, 1, 3, 0}, {0x1FFC000E1, 0x1FFC000E, 0x3FF8001C, 0x7FF0003}};
    for (int i = 0; i < NUM_ADRS; i++) {
        for (int j = 0; j < NUM_COMB;j++) {
            int got = extractTagBits(adrs[i], bits[j], sets[j]);
            int want = exp[i][j]; 
            if (want != got) {
                printf("- [ FAIL ]:\n");
                printf("  Address: %lx, sets: %d, bits: %d, got TAG: %u, wanted: %u\n", adrs[i], sets[j], bits[j], got, want);
            }else printf("- [ PASS ]\n");
        }
    }
}
