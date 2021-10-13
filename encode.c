#include "bm.h"
#include "bv.h"
#include "hamming.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define OPTIONS "hi:o:"

// Helper functions from asgn5.pdf
uint8_t lower_nible(uint8_t val) { // Returns lower nibble of val
    return val & 0xF;
}

uint8_t upper_nible(uint8_t val) { // Returns upper nibble of val
    return val >> 4;
}

int main(int argc, char **argv) {
    int opt = 0;
    FILE *infile = stdin;
    FILE *outfile = stdout;
    //struct stat statbuf;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h':
            // print help messag
            fprintf(outfile,
                "Synopsis: A Hamming (8,4) systematic code generator.\nUsage: ./encode "
                "[-h] [-i infile] [-o outfile]\nOptions: -h: program usage and help, -i "
                "input: specifies input file, -o output: specifies output file\n");
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
        default: printf("Invalid command-line option.\n"); return 0;
        }
    }

    struct stat statbuf;
    fstat(fileno(infile), &statbuf);
    fchmod(fileno(outfile), statbuf.st_mode);
    // initialize generator matrix G
    BitMatrix *G = bm_create(4, 8);
    bm_set_bit(G, 0, 0);
    bm_set_bit(G, 0, 5);
    bm_set_bit(G, 0, 6);
    bm_set_bit(G, 0, 7);
    bm_set_bit(G, 1, 1);
    bm_set_bit(G, 1, 4);
    bm_set_bit(G, 1, 6);
    bm_set_bit(G, 1, 7);
    bm_set_bit(G, 2, 2);
    bm_set_bit(G, 2, 4);
    bm_set_bit(G, 2, 5);
    bm_set_bit(G, 2, 7);
    bm_set_bit(G, 3, 3);
    bm_set_bit(G, 3, 4);
    bm_set_bit(G, 3, 5);
    bm_set_bit(G, 3, 6);

    int16_t byte;
    while ((byte = fgetc(infile)) != EOF) {
        uint8_t lower = lower_nible(byte);
        uint8_t upper = upper_nible(byte);
        uint8_t code1 = ham_encode(G, lower);
        uint8_t code2 = ham_encode(G, upper);
        fputc(code1, outfile);
        fputc(code2, outfile);
    }

    fclose(infile);
    fclose(outfile);
    bm_delete(&G);

    return 0;
}
