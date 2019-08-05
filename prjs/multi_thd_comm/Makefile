APP = thd_comm_poll.elf
OBJS = thd_comm_poll.o circle_queue_array.o
CFLAGS = -pthread -Wall

$(APP): $(OBJS)
	$(CC) -o $(APP) $(OBJS) $(CFLAGS)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< -g $(CFLAGS)

.PHONY: clean
clean: 
	rm -rf $(APP) $(OBJS)
