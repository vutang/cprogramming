TARGET = unit_test_stack_array.elf

OBJS = stack_array.o unit_test_stack_array.o
HEADER = stack.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) 

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< -g

clean:
	rm -rf *.o *.elf