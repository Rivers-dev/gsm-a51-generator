import random

R1_MASK_19BIT = 0x07FFFF
R2_MASK_22BIT = 0x3FFFFF
R3_MASK_23BIT = 0x7FFFFF
R1_CLOCK_CONTROL_BIT = 0x000100
R2_CLOCK_CONTROL_BIT = 0x000400
R3_CLOCK_CONTROL_BIT = 0x000400
R1_FEEDBACK_TAPS = 0x072000
R2_FEEDBACK_TAPS = 0x300000
R3_FEEDBACK_TAPS = 0x700080
R1_OUTPUT_BIT = 0x040000 
R2_OUTPUT_BIT = 0x200000
R3_OUTPUT_BIT = 0x400000

reg1 = reg2 = reg3 = 0

def parity(x: int) -> int:
    x ^= x >> 16
    x ^= x >> 8
    x ^= x >> 4
    x ^= x >> 2
    x ^= x >> 1
    return x & 1

def clock_register(reg: int, mask: int, taps: int) -> int:
    feedback = parity(reg & taps)
    reg = (reg << 1) & mask
    reg |= feedback
    return reg

def clock_all():
    global reg1, reg2, reg3 # CHANGEME (implement class)
    reg1 = clock_register(reg1, R1_MASK_19BIT, R1_FEEDBACK_TAPS)
    reg2 = clock_register(reg2, R2_MASK_22BIT, R2_FEEDBACK_TAPS)
    reg3 = clock_register(reg3, R3_MASK_23BIT, R3_FEEDBACK_TAPS)

# The majority of bits that have a 1 on their clocking bit determines if they get clocked or not 
def majority_vote() -> int:
    bits = [
        parity(reg1 & R1_CLOCK_CONTROL_BIT),
        parity(reg2 & R2_CLOCK_CONTROL_BIT),
        parity(reg3 & R3_CLOCK_CONTROL_BIT)
    ]
    return 1 if sum(bits) >= 2 else 0

def conditional_clock():
    global reg1, reg2, reg3
    maj = majority_vote()
    if ((reg1 & R1_CLOCK_CONTROL_BIT) != 0) == bool(maj):
        reg1 = clock_register(reg1, R1_MASK_19BIT, R1_FEEDBACK_TAPS)
    if ((reg2 & R2_CLOCK_CONTROL_BIT) != 0) == bool(maj):
        reg2 = clock_register(reg2, R2_MASK_22BIT, R2_FEEDBACK_TAPS)
    if ((reg3 & R3_CLOCK_CONTROL_BIT) != 0) == bool(maj):
        reg3 = clock_register(reg3, R3_MASK_23BIT, R3_FEEDBACK_TAPS)

def generate_bit() -> int:
    return (
        parity(reg1 & R1_OUTPUT_BIT) ^
        parity(reg2 & R2_OUTPUT_BIT) ^
        parity(reg3 & R3_OUTPUT_BIT)
    )

# Initial setup to get the right valuesin the registers and then clock the majority bits
def initialize_registers(key: bytes, frame: int):
    global reg1, reg2, reg3
    reg1 = reg2 = reg3 = 0

    for i in range(64):
        keybit = (key[i // 8] >> (i % 8)) & 1
        clock_all()
        reg1 ^= keybit
        reg2 ^= keybit
        reg3 ^= keybit

    for i in range(22):
        framebit = (frame >> i) & 1
        clock_all()
        reg1 ^= framebit
        reg2 ^= framebit
        reg3 ^= framebit

    # Last step to clock based on maj bits
    for _ in range(100):
        conditional_clock()

def generate_keystream() -> tuple[bytearray, bytearray]:
    AtoB = bytearray(15)
    BtoA = bytearray(15)

    for i in range(114):
        conditional_clock()
        AtoB[i // 8] |= generate_bit() << (7 - (i % 8))

    for i in range(114):
        conditional_clock()
        BtoA[i // 8] |= generate_bit() << (7 - (i % 8))

    return AtoB, BtoA

def xor_keystream(message: bytes, keystream: bytes) -> bytes:
    return bytes(m ^ k for m, k in zip(message, keystream))

def encrypt_message(message: bytes, key: bytes, frame: int, direction='AtoB') -> bytes:
    initialize_registers(key, frame)
    AtoB, BtoA = generate_keystream()
    ks = AtoB if direction == 'AtoB' else BtoA
    return xor_keystream(message, ks)

def decrypt_message(ciphertext: bytes, key: bytes, frame: int, direction='AtoB') -> bytes:
    return encrypt_message(ciphertext, key, frame, direction)

# replace this
def pretty_print(name: str, data: bytes):
    hextxt = ''.join(f'{byte:02X}' for byte in data)
    print(f"{name}: 0x{hextxt}")

# TEST CASE CODE (put this in a different file later)
def generate_key() -> bytes:
    return bytes(random.getrandbits(8) for _ in range(8))

def generate_frame() -> int:
    return random.getrandbits(22)  # Frame number is 22-bit

def generate_message(min_len=5, max_len=15) -> bytes:
    length = random.randint(min_len, max_len)
    return bytes(random.getrandbits(8) for _ in range(length))

def tests(n=10):
    for i in range(n):
        print(f"\n### Randomized Test Case {i+1} ###")

        key = generate_key()
        frame = generate_frame()
        message = generate_message()

        pretty_print("Plaintext", message)
        pretty_print("Kc", key)
        print(f"Frame: {frame}")

        ciphertext = encrypt_message(message, key, frame, direction='AtoB')
        pretty_print("Generated cipher", ciphertext)

        decrypted = decrypt_message(ciphertext, key, frame, direction='AtoB')
        pretty_print("Decrypted plaintext", decrypted)

        assert decrypted == message, f"Mismatch in test case {i + 1}!"
        # Text highlighting, remove if it causes compatibility issues with your terminal emulator
        print(f"{'\033[92m'}Assertion passed!{'\033[0m'}")

if __name__ == '__main__':
    tests()
