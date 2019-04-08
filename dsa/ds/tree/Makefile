
TARGET = unit_test_binary_tree.elf

OBJS = binary_tree.o unit_test_binary_tree.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< -g $(LIBS)

clean:
	rm -rf *.o *.elf