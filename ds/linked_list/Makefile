TARGET = list-kernel-test.elf

OBJS = list-kernel-test.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) 

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< -g

clean:
	rm -rf *.o *.elf