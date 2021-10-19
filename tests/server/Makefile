vpath %.h ../../src/server
vpath %.c ../../src/server

vpath %.h ../../src/utils
vpath %.c ../../src/utils

vpath %.h ../../src/third-pard/sockets
vpath %.c ../../src/third-pard/sockets

CC=gcc
CFLAGS=-Wall


all: socks5Test

objects = socks5Test.o socks5.o server.o logger.o rdwrn.o handler.o io.o

socks5Test: $(objects)
	cc $(objects) -g -o socks5Test

socks5Test.o: socks5Test.c
	cc -g -c -o socks5Test.o socks5Test.c

socks5.o: socks5.c socks5.h handler.h rdwrn.h io.h
	cc -g -c -o socks5.o $<

server.o: server.c server.h
	cc -g -c -o server.o $<

logger.o: logger.c logger.h
	cc -g -c -o logger.o $<

rdwrn.o: rdwrn.c rdwrn.h
	cc -g -c -o rdwrn.o $<

handler.o: handler.c handler.h socks5.h io.h
	cc -g -c -o handler.o $<

io.o: io.c io.h logger.h
	cc -g -c -o io.o $<

clean:
	rm *.o