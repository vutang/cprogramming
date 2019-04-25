TARGET = multi_threading_sorting.elf
OBJS = multi_threading_sorting.o
LDFLAGS = -pthread


all: multi_threading_sorting

multi_threading_sorting: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< -g

clean:
	rm -rf *.o *.elf