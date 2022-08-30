CC = gcc

CFLAGS = -Wall -O2 -I .
LDLIBS = -lpthread

PROGS =	echoclient \
	echoserveri

all: $(PROGS) 
$(PROGS): csapp.o
csapp.o: csapp.c csapp.h

echoclient: echoclient.o csapp.o
	gcc $(CFLAGS) -o echoclient echoclient.o csapp.o $(LDLIBS)

echoserveri: echoserveri.o csapp.o echo.o
	gcc $(CFLAGS) -o echoserveri echoserveri.o echo.o csapp.o $(LDLIBS)

clean:
	rm -f $(PROGS) *.o *~
