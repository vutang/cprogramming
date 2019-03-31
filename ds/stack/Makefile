TARGET = stack_unit_test.elf

OBJS = stack.o stack_unit_test.o
HEADER = stack.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) 

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< -g

clean:
	rm -rf *.o *.elf