TARGET = multi_threading_sorting.elf
OBJS = multi_threading_sorting.o
LDFLAGS = -pthread

USR_LIB = -lsort
LIB_DIR = /media/vutt6/linuxwrk/programming/dsa/alg/sorting/


all: multi_threading_sorting

multi_threading_sorting: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS) -L$(LIB_DIR) $(USR_LIB)

%.o: %.c
	$(CC) -c -o $@ $< -g -L$(LIB_DIR) $(USR_LIB)

clean:
	rm -rf *.o *.elf