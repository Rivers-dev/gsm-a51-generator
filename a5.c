#include <stdio.h>

/* 
Given a plaintext frame (114 bits in both directions, so 228 bits total), a frame counter Fn (22 bits), and
a symmetric session key Kc, we should generate a 228 bit frame of cipher text

1. Use generative function on Kc and Fn to generate a PRAND (using LFSRs) intermediate cipher (228 bits)
2. XOR PRAND with plaintext
*/

int main() {

}