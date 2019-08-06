APP = logger_demo.elf
OBJS = logger.o logger_demo.o
HEADERS = logger.h

$(APP): $(OBJS)
	$(CC) -o $(APP) $(OBJS)

%o: %d $(HEADERS)
	$(CC) -c -o $@ $< -g 

.PHONY: clean
clean: 
	rm -rf $(APP) $(OBJS)