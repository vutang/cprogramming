
COMMON_OBJS = sort.o util.o

APP1 = unit_test_selection_sort
TARGET1 = $(APP1).elf
OBJS1 = $(APP1).o

TARGET2 = ran_generate.elf
OBJS2 = ran_generate.o

TARGET3 = unit_test_bubble_sort.elf
OBJS3 = unit_test_bubble_sort.o

TARGET4 = unit_test_merge_sort.elf
OBJS4 = unit_test_merge_sort.o merge-sort.o

APP5 = unit_test_quick_sort
TARGET5 = $(APP5).elf
OBJS5 = $(APP5).o quick-sort.o

APP6 = unit_test_count_sort
TARGET6 = $(APP6).elf
OBJS6 = $(APP6).o count-sort.o

all: $(TARGET1) $(TARGET2) $(TARGET3) $(TARGET4) $(TARGET5) $(TARGET6)

$(TARGET1): $(COMMON_OBJS) $(OBJS1)
	$(CC) -o $(TARGET1) $(OBJS1) $(COMMON_OBJS)

$(TARGET2): $(OBJS2)
	$(CC) -o $(TARGET2) $(OBJS2)

$(TARGET3): $(COMMON_OBJS) $(OBJS3)
	$(CC) -o $(TARGET3) $(OBJS3) $(COMMON_OBJS)

$(TARGET4): $(COMMON_OBJS) $(OBJS4)
	$(CC) -o $(TARGET4) $(OBJS4) $(COMMON_OBJS)

$(TARGET5): $(COMMON_OBJS) $(OBJS5)
	$(CC) -o $(TARGET5) $(OBJS5) $(COMMON_OBJS)

$(TARGET6): $(COMMON_OBJS) $(OBJS6)
	$(CC) -o $(TARGET6) $(OBJS6) $(COMMON_OBJS)

%.o: %.c $(HEADERS)
	$(CC) -c -o $@ $< -g $(LIBS)

clean:
	rm -rf *.elf *.o *.dat