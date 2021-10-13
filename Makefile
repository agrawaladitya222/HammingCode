CC = clang
CFLAGS = -Wall -Werror -Wpedantic -Wextra
LFLAGS = -lm

all: encode decode error entropy

encode: encode.o bm.o hamming.o bv.o
	$(CC) -o encode encode.o bm.o hamming.o bv.o $(LFLAGS)

decode: decode.o bm.o hamming.o bv.o
	$(CC) -o decode decode.o bm.o hamming.o bv.o $(LFLAGS)

error:
	$(CC) $(CFLAGS) -o error error.c $(LFLAGS)

entropy:
	$(CC) $(CFLAGS) -o entropy entropy.c $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f encode decode error entropy *.o

format:
	clang-format -i -style=file *.c *.h
