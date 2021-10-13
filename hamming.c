#include "hamming.h"

#include "bm.h"
#include "bv.h"

#include <stdint.h>

// Helper functions from asgn5.pdf
uint8_t lower_nibble(uint8_t val) { // Returns lower nibble of val
    return val & 0xF;
}

uint8_t upper_nibble(uint8_t val) { // Returns upper nibble of val
    return val >> 4;
}

uint8_t pack_byte(uint8_t upper, uint8_t lower) { // Packs two nibbles into a byte
    return (upper << 4) | (lower & 0xf);
}

uint8_t ham_encode(BitMatrix *G, uint8_t msg) {
    BitMatrix *c
        = bm_multiply(bm_from_data(lower_nibble(msg), 4), G); // code = lower nibble of byte*G
    return bm_to_data(c);
}

HAM_STATUS ham_decode(BitMatrix *Ht, uint8_t code, uint8_t *msg) {
    int32_t lookup[] = { HAM_OK, 4, 5, HAM_ERR, 6, HAM_ERR, HAM_ERR, 3, 7, HAM_ERR, HAM_ERR, 2,
        HAM_ERR, 1, 0, HAM_ERR };
    BitMatrix *codev = bm_from_data(code, 8);
    BitMatrix *error = bm_multiply(codev, Ht);
    if (lookup[bm_to_data(error)] == HAM_OK) {
        *msg = lower_nibble(code);
        return HAM_OK;
    } else if (lookup[bm_to_data(error)] == HAM_ERR) {
        return HAM_ERR;
    } else {
        code ^= (1 << lookup[bm_to_data(error)]);
        *msg = lower_nibble(code);
        return HAM_CORRECT;
    }
}
