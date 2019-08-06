APP = thd_comm_poll.elf
OBJS = thd_comm_poll.o circle_queue_array.o ../logger/logger.o
CFLAGS = -pthread -Wall
HEADERS = circle_queue_array.h

$(APP): $(OBJS)
	$(CC) -o $(APP) $(OBJS) $(CFLAGS)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< -g $(CFLAGS)

.PHONY: clean
clean: 
	rm -rf $(APP) $(OBJS)
