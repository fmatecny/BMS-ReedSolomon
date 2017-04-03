CC = gcc
# OPTIMIZE_FLAGS = -O69
DEBUG_FLAGS = -g
CFLAGS = -std=c99 -Wall -pedantic -Wstrict-prototypes  $(OPTIMIZE_FLAGS) $(DEBUG_FLAGS) -I..


all:bms1A.o bms1B.o
	gcc -o bms1A bms1A.o -L. -lecc -lm
	gcc -o bms1B bms1B.o -L. -lecc -lm

clean:
	rm -f *.o bms1A bms1B

zip:
	zip xmatec00.zip bms1A.c bms1B.c libecc.a Makefile ecc.h
