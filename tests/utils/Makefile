vpath %.h ../../src/utils
vpath %.c ../../src/utils

CC=gcc
CFLAGS=-Wall


all: loggerTest

loggerTest: loggerTest.o logger.o
	cc logger.o loggerTest.o -g -o loggerTest

logger.o: logger.c logger.h
	cc -g -c -o logger.o $<

loggerTest.o: loggerTest.c
	cc -g -c loggerTest.c -o loggerTest.o

clean:
	rm *.o