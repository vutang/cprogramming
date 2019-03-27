
TARGET = circle_queue_test.elf
OBJS = circle_queue.o circle_queue_test.o
HEADERS = circle_queue.h
LDFLAGS = -g


all: circle_queue_test

circle_queue_test: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< -g

clean:
	rm -rf *.o *.elf