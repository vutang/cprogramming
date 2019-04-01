TARGET = unit_test_stack_array.elf
TARGET1 = unit_test_stack_list.elf

OBJS = stack_array.o unit_test_stack_array.o
OBJS1 = stack_list.o unit_test_stack_list.o
LIBS = -lsll
HEADERS = stack.h stack_list.h

all: $(TARGET) $(TARGET1)

$(TARGET): $(OBJS)
	$(CC) -L../linked_list/ -o $(TARGET) $(OBJS) $(LIBS)

$(TARGET1): $(OBJS1)
	$(CC) -L../linked_list/ -o $(TARGET1) $(OBJS1) $(LIBS)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< -g $(LIBS)

clean:
	rm -rf *.o *.elf