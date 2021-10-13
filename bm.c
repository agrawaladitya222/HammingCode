#include "bm.h"

#include "bv.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct BitMatrix {
    uint32_t rows;
    uint32_t cols;
    BitVector *vector;
} BitMatrix;

BitMatrix *bm_create(uint32_t rows, uint32_t cols) {
    BitMatrix *bm = (BitMatrix *) malloc(sizeof(BitMatrix));
    if (bm) {
        bm->rows = rows;
        bm->cols = cols;
        bm->vector = bv_create(rows * cols);
        if (!bm->vector) {
            free(bm);
            bm = NULL;
        }
    }

    return bm;
}

void bm_delete(BitMatrix **m) {
    if (*m) {
        bv_delete(&(*m)->vector);
        free(*m);
        *m = NULL;
    }

    return;
}

uint32_t bm_rows(BitMatrix *m) {
    return m->rows;
}

uint32_t bm_cols(BitMatrix *m) {
    return m->cols;
}

void bm_set_bit(BitMatrix *m, uint32_t r, uint32_t c) {
    bv_set_bit(m->vector, (r * (m->cols) + c));
    return;
}

void bm_clr_bit(BitMatrix *m, uint32_t r, uint32_t c) {
    bv_clr_bit(m->vector, (r * (m->cols) + c));
    return;
}

uint8_t bm_get_bit(BitMatrix *m, uint32_t r, uint32_t c) {
    return bv_get_bit(m->vector, (r * (m->cols) + c));
}

BitMatrix *bm_from_data(uint8_t byte, uint32_t length) {
    if (length > 8) {
        printf("something went wrong, bm_from_data length>8");
        exit(1);
    }
    BitMatrix *m = bm_create(1, length);

    for (uint32_t i = 0; i < length; i += 1) {
        if ((((1 << i) & byte) >> i) == 1) { // if bit in index i of byte is 1
            bm_set_bit(m, 0, i);
        }
    }
    return m;
}

uint8_t bm_to_data(BitMatrix *m) {
    uint8_t result = 0;
    for (uint32_t i = 0; i < 8; i += 1) {
        result |= bv_get_bit(m->vector, i) << i;
    }

    return result;
}

BitMatrix *bm_multiply(BitMatrix *A, BitMatrix *B) {
    if (bm_cols(A) != bm_rows(B)) {
        printf("Something went wrong, bm_multiply error.\n");
        exit(1);
    }
    BitMatrix *output = bm_create(A->rows, B->cols);

    for (uint32_t r = 0; r < output->rows; r += 1) {
        for (uint32_t c = 0; c < output->cols; c += 1) {
            for (uint32_t k = 0; k < A->cols; k += 1) {
                uint8_t bit = bm_get_bit(A, r, k) * bm_get_bit(B, k, c);
                bv_xor_bit(output->vector, (r * (output->cols) + c), bit);
                // xors/sets output bit (output,r,c) with value of multiplication between (A,r,k) and (B,k,c)
            }
        }
    }
    return output;
}

void bm_print(BitMatrix *m) {
    for (uint32_t r = 0; r < m->rows; r += 1) {
        printf("[ ");
        for (uint32_t c = 0; c < m->cols; c += 1) {
            printf("%" PRIu8 " ", bm_get_bit(m, r, c));
        }
        printf("]\n");
    }
    return;
}

/*
int main(void) {
	BitMatrix *A = bm_create(3,3);
	BitMatrix *B = bm_create(3,3);
	
	for(uint32_t i=0; i< bm_rows(A); i+=1) {
		for(uint32_t j=0; j< bm_cols(A); j+=1) {
			bm_set_bit(A, i, j);
		}
	}
	bm_clr_bit(A,1,1);
	bm_clr_bit(A,2,2);
	bm_clr_bit(A,0,0);
	bm_print(A);
	printf("\n");

	for(uint32_t i=0; i< bm_rows(B); i+=1) {
                for(uint32_t j=0; j< bm_cols(B); j+=1) {
                        bm_set_bit(B, i, j);
                }
        }
        bm_clr_bit(B,0,1);
        bm_clr_bit(B, 1, 0);
        bm_print(B);
	printf("\n");

	BitMatrix *C = bm_multiply(A,B);
	bm_print(C);
	printf("\n");
	//uint8_t byte = 127;
	//bm_print( bm_from_data(byte, 8));
	printf("\n");
	bm_print(A);
	printf("\n");
	bv_print(A->vector);
	printf("\n\n");

	uint8_t temp = bm_to_data(A);
	printf("%"PRIu8 "\n", temp);
	printf("%"PRIu8 "\n", bm_to_data(B));
	return 0;
}
*/
