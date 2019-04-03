
COMMON_OBJS = sort.o

TARGET1 = unit_test_selection_sort.elf
OBJS1 = unit_test_selection_sort.o

TARGET2 = ran_generate.elf
OBJS2 = ran_generate.o

TARGET3 = unit_test_bubble_sort.elf
OBJS3 = unit_test_bubble_sort.o

all: $(TARGET1) $(TARGET2) $(TARGET3)

$(TARGET1): $(COMMON_OBJS) $(OBJS1)
	$(CC) -o $(TARGET1) $(OBJS1) $(COMMON_OBJS)

$(TARGET2): $(OBJS2)
	$(CC) -o $(TARGET2) $(OBJS2)

$(TARGET3): $(COMMON_OBJS) $(OBJS3)
	$(CC) -o $(TARGET3) $(OBJS3) $(COMMON_OBJS)

clean:
	rm -rf *.elf *.o *.dat