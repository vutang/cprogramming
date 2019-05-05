CC = gcc

TAR = externalSort.elf
SRC = externalSort.c
# LIB = heapSort.c  heapSort.h

LIB_DIR = /media/vutt6/linuxwrk/programming/dsa/alg/sorting/
LIBS = -lm -lsort

all:	
	$(CC) -o $(TAR) $(SRC) -L$(LIB_DIR) $(LIBS)
clean:
	rm $(TAR)