TARGET=e24-2

$(TARGET): $(TARGET).o
	$(CC) -o $(TARGET).elf $^

%.o: %.c 
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -rf *.o *.elf