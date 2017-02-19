CC=gcc
CFLAGS=-O3 -fPIC
WDIR=/home/ubuntu/workspace/Malloc/Code

all: thread_test

thread_test: thread_test.c
	$(CC) $(CFLAGS) -I$(WDIR) -L$(WDIR) -Wl,-rpath=$(WDIR) -o $@ thread_test.c -lmymalloc -lpthread

clean:
	rm -f *~ *.o thread_test

clobber:
	rm -f *~ *.o