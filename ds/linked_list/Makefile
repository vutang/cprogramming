TARGET = unit_test_ll_singly
OBJS = unit_test_ll_singly.o ll_singly.o
HEADERS = ll_singly.h

all: $(TARGET) libsll.a

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) 

libsll.a:
	ar rcs libsll.a ll_singly.o

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< -g

clean:
	rm -rf *.o *.elf