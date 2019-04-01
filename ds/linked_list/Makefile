TARGET = unit_test_ll_singly
OBJS = unit_test_ll_singly.o ll_singly.o
HEADERS = ll_singly.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) 

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< -g

clean:
	rm -rf *.o *.elf