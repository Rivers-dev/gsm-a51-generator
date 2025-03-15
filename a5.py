import os
import sys
import random
from dataclasses import dataclass

@dataclass(frozen=True)
class lfsr:
    KEY_SIZE = 64
    FN_SIZE = 22

    R1_SIZE = 19
    R2_SIZE = 22
    R3_SIZE = 23
    R1_CLOCK_BIT = 8
    R2_CLOCK_BIT = 10
    R3_CLOCK_BIT = 10

    KEYSTREAM_SIZE = 114

def generate_kc() -> int:
    return random.randint(0, (1 << lfsr.KEY_SIZE) - 1)

def generate_fn() -> int:
    return random.randint(0, (1 << lfsr.FN_SIZE) - 1)

def preprocess_regs(int) -> tuple[int, int, int]:
    for digit in str(lfsr.KEY_SIZE): #fix
        print("unimplemented")



if __name__ == "__main__":
    if len(sys.argv) == 1:
        print("Generating cipher stream\n")

        kc = generate_kc()
        fn = generate_fn()

        regs = preprocess_regs(kc)

        print("Key: ", bin(kc), "\nFrame size: ", bin(fn), "\n")
        print("Registers: ", regs)

        
    else:
        print("incorrect args\n")