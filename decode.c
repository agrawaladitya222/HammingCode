#include "bm.h"
#include "bv.h"
#include "hamming.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define OPTIONS "hi:o:v"

// Helper functions from asgn5.pdf
uint8_t pack_bye(uint8_t upper, uint8_t lower) { // Packs two nibbles into a byte
    return (upper << 4) | (lower & 0xf);
}

int main(int argc, char **argv) {
    int opt = 0;
    FILE *infile = stdin;
    FILE *outfile = stdout;
    bool stats = false;
    //struct stat statbuf;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h':
            // print help message
            fprintf(outfile, "Synopsis: A Hamming(8, 4) systematic code decoder\nUsage: ./decode "
                             "[-h] [-v] [-i infile] [-o outfile]\nOptions: -h: Program usage and "
                             "help, -v: Print statistics of decoding to stderr, -i infile: Input "
                             "data to decode, -o outfile: Output of decoded data\n");
            return 0;
        case 'i':
            infile = fopen(optarg, "r");
            if (infile == NULL) {
                printf("Error, invalid file or something.\n");
                return 0;
            }
            //fstat(fileno(infile), &statbuf);
            break;
        case 'o':
            outfile = fopen(optarg, "w");
            //fchmod(fileno(outfile), statbuf.st_mode);
            break;
        case 'v': stats = true; break;

        default: printf("Invalid command-line option.\n"); return 0;
        }
    }

    struct stat statbuf;
    fstat(fileno(infile), &statbuf);
    fchmod(fileno(outfile), statbuf.st_mode);

    // initialize transpose of decoder matrix
    BitMatrix *Ht = bm_create(8, 4);
    bm_set_bit(Ht, 0, 1);
    bm_set_bit(Ht, 0, 2);
    bm_set_bit(Ht, 0, 3);
    bm_set_bit(Ht, 1, 0);
    bm_set_bit(Ht, 1, 2);
    bm_set_bit(Ht, 1, 3);
    bm_set_bit(Ht, 2, 0);
    bm_set_bit(Ht, 2, 1);
    bm_set_bit(Ht, 2, 3);
    bm_set_bit(Ht, 3, 0);
    bm_set_bit(Ht, 3, 1);
    bm_set_bit(Ht, 3, 2);
    bm_set_bit(Ht, 4, 0);
    bm_set_bit(Ht, 5, 1);
    bm_set_bit(Ht, 6, 2);
    bm_set_bit(Ht, 7, 3);

    int16_t code1;
    int16_t code2;
    int corrected = 0, processed = 0, uncorrected = 0;
    while ((code1 = fgetc(infile)) != EOF && (code2 = fgetc(infile)) != EOF) {
        uint8_t msg1;
        uint8_t msg2;
        switch (ham_decode(Ht, code1, &msg1)) {
        case HAM_OK: processed += 1; break;
        case HAM_CORRECT:
            processed += 1;
            corrected += 1;
            break;
        case HAM_ERR:
            uncorrected += 1;
            processed += 1;
            break;
        default: break;
        }

        switch (ham_decode(Ht, code2, &msg2)) {
        case HAM_OK: processed += 1; break;
        case HAM_CORRECT:
            processed += 1;
            corrected += 1;
            break;
        case HAM_ERR:
            uncorrected += 1;
            processed += 1;
            break;
        default: break;
        }

        uint8_t packed_byte = pack_bye(msg2, msg1);
        fputc(packed_byte, outfile);
    }
    if (stats) {
        fprintf(stderr, "Total bytes processed: %d\n", processed);
        fprintf(stderr, "Uncorrected errors: %d\n", uncorrected);
        fprintf(stderr, "Corrected errors: %d\n", corrected);
        fprintf(stderr, "Error rate: %1.6f\n", (float) uncorrected / processed);
    }

    fclose(infile);
    fclose(outfile);
    bm_delete(&Ht);

    return 0;
}
