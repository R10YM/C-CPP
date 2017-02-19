CC=gcc
CFLAGS=-O3 -fPIC
MALLOC_VERSION=BF
WDIR=/home/ubuntu/workspace/Malloc/Code

all: mymalloc_test

mymalloc_test: mymalloc_test.c
	$(CC) $(CFLAGS) -I$(WDIR) -L$(WDIR) -D$(MALLOC_VERSION) -Wl,-rpath,$(WDIR) -o $@ mymalloc_test.c -lmymalloc

clean:
	rm -f *~ *.o mymalloc_test

clobber:
	rm -f *~ *.o
