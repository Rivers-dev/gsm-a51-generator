#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <inttypes.h>

#define R1_SIZE 19 // Registers have unique sizes
#define R2_SIZE 22
#define R3_SIZE 23
#define KEY_SIZE 64 // Kc key size (bits)
#define FRAME_SIZE 22 // Fn frame counter size (bits)
#define KEYSTREAM_SIZE 114
#define R1_CLOCK_BIT 8 // Clocking bits to determine majority rule in PRAND generation
#define R2_CLOCK_BIT 10
#define R3_CLOCK_BIT 10

/* 
Given a plaintext frame (114 bits in both directions, so 228 bits total), a frame counter Fn (22 bits), and
a symmetric session key Kc, we should generate a 228 bit frame of cipher text

1. Use generative function on Kc and Fn to generate a PRAND (using LFSRs) intermediate cipher (228 bits)
2. XOR PRAND with plaintext
*/

void binary(uint32_t value) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (value >> i) & 1);
        if (i % 4 == 0) printf(" ");
    }
    printf("\n");
}

// Generate random Kc symmetric key 
uint64_t generate_kc() {
    return ((uint64_t)rand() << KEY_SIZE/2) | rand();
}

// Generate random 22 bit frame counter
uint32_t generate_fn() {
    return rand() & ((1U << FRAME_SIZE) - 1);
}

// Side effects: modifies registers
void initialize_regs(uint32_t *R1, uint32_t *R2, uint32_t *R3, uint64_t kc) {
    for (int i = 0; i < KEY_SIZE; i++) {
        uint64_t kcBit = (kc >> i) & 1;

        // Clock R1
        uint32_t feedback = ((*R1 >> 18) ^ (*R1 >> 17) ^ (*R1 >> 16) ^ (*R1 >> 13)) & 1;
        *R1 = ((*R1 << 1) | (feedback ^ kcBit)) & 0x7FFFF; 

        // Clock R2
        feedback = ((*R2 >> 21) ^ (*R2 >> 20)) & 1;
        *R2 = ((*R2 << 1) | (feedback ^ kcBit)) & 0x3FFFFF;

        // Clock R3
        feedback = ((*R3 >> 22) ^ (*R3 >> 21) ^ (*R3 >> 20) ^ (*R3 >> 7)) & 1;
        *R3 = ((*R3 << 1) | (feedback ^ kcBit)) & 0x7FFFFF; 
    }
}


int main() {
    srand(time(NULL)); // Rand is not secure; use platform specific cryptographic number generator
    uint64_t kc = generate_kc();
    uint32_t fn = generate_fn();

    // LFSR register initialization
    uint32_t R1 = 0, R2 = 0, R3 = 0;

    initialize_regs(&R1, &R2, &R3, kc);

    printf("Kn: %llu\n", kc);
    printf("Fn: %u\n", fn);
    binary(R1);

    return 0;
}