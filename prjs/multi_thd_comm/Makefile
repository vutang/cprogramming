APP = thd_comm.elf
OBJS = thd_comm.o
CFLAGS = -pthread -Wall

$(APP): $(OBJS)
	$(CC) -o $(APP) $(OBJS) $(CFLAGS)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< -g $(CFLAGS)

.PHONY: clean
clean: 
	rm -rf $(APP) $(OBJS)
