
TARGET = circle_queue_test.elf
OBJS = circle_queue.o circle_queue_test.o
HEADERS = circle_queue.h


all: circle_queue_test

circle_queue_test: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

%.o: %.c $(HEADERS)