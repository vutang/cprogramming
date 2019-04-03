TARGET1 = unit_test_selection_sort.elf
OBJS1 = sort.o unit_test_selection_sort.o

TARGET2 = ran_generate.elf
OBJS2 = ran_generate.o

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJS1)
	$(CC) -o $(TARGET1) $(OBJS1)

$(TARGET2): $(OBJS2)
	$(CC) -o $(TARGET2) $(OBJS2)

clean:
	rm -rf *.elf *.o *.dat