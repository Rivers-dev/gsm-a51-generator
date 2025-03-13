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
        // Extract first bit from key
        uint64_t firstBitMask = 0x8000000000000000;
        uint64_t kcFirstBit = (firstBitMask >> 63) & 1;

        // Clock R1
        uint32_t firstR1Bit = (*R1 >> 18) & 1;
        uint32_t secondR1Bit = (*R1 >> 17) & 1;
        uint32_t thirdR1Bit = (*R1 >> 16) & 1;
        uint32_t fourthR1Bit = (*R1 >> 13) & 1;

        uint32_t feedback = ((((firstR1Bit ^ secondR1Bit) ^ thirdR1Bit) ^ fourthR1Bit) ^ kcFirstBit);
        feedback &= 1; // Bit operation validity check
        *R1 = ((*R1 & 0x7FFFF) << 1) | feedback; // Push the feedback bit onto the end of the R1 register
    
        // Clock R2
        uint32_t firstR2Bit = (*R2 >> 21) & 1;
        uint32_t secondR2Bit = (*R2 >> 20) & 1;

        feedback = (firstR2Bit ^ secondR2Bit) ^ kcFirstBit;
        feedback &= 1;
        *R2 = ((*R2 & 0x3FFFFF) << 1) | feedback;

        // Clock R3
        uint32_t firstR3Bit = (*R3 >> 22) & 1;
        uint32_t secondR3Bit = (*R3 >> 21) & 1;
        uint32_t thirdR3Bit = (*R3 >> 20) & 1;
        uint32_t fourthR3Bit = (*R3 >> 7) & 1;

        feedback = ((((firstR3Bit ^ secondR3Bit) ^ thirdR3Bit) ^ fourthR3Bit) ^ kcFirstBit);
        feedback &= 1;
        *R3 = ((*R1 & 0x7FFFFF) << 1) | feedback;
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