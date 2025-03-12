#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <inttypes.h>

#define R1_SIZE 19
#define R2_SIZE 22
#define R3_SIZE 23
#define KEY_SIZE 64
#define FRAME_SIZE 22
#define KEYSTREAM_SIZE 114
#define R1_CLOCK_BIT 8
#define R2_CLOCK_BIT 10
#define R3_CLOCK_BIT 10

/* 
Given a plaintext frame (114 bits in both directions, so 228 bits total), a frame counter Fn (22 bits), and
a symmetric session key Kc, we should generate a 228 bit frame of cipher text

1. Use generative function on Kc and Fn to generate a PRAND (using LFSRs) intermediate cipher (228 bits)
2. XOR PRAND with plaintext
*/

// Generate kc symmetric key 
uint64_t generate_kc() {
    return ((uint64_t)rand() << 32) | rand();
}

// Generate random 22 bit frame counter
uint32_t generate_fn() {
    return rand() & ((1U << 22) - 1);
}

int main() {
    srand(time(NULL)); // Rand is not secure; use platform specific cryptographic number generator
    uint64_t kc = generate_kc();
    uint32_t fn = generate_fn();
    printf("Key: 0x%llu\n", kc);
    printf("Frame: %u\n", fn);

    return 0;
}