CC = gcc

TAR = external_sort_2
# SRC = externalSort.c
# LIB = heapSort.c  heapSort.h

LIB_DIR = /media/vutt6/linuxwrk/programming/dsa/alg/sorting/
HDR_DIR = /media/vutt6/linuxwrk/programming/dsa/alg/sorting/
LIBS = -lm -lsort

all:
	$(CC) -o $(TAR).elf $(TAR).c -L$(LIB_DIR) $(LIBS) -I$(HDR_DIR)
clean:
	rm $(TAR).elf