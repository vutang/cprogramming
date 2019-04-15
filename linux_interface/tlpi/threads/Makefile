include ../Makefile.inc

TARGET = simple_thread
LIBDIR = ../lib
OBJS = 	$(LIBDIR)/error_functions.o \
		$(LIBDIR)/get_num.o \
		$(LIBDIR)/curr_time.o \
		$(LIBDIR)/print_wait_status.o


$(TARGET): $(TARGET).o $(OBJS)
	$(CC) -o $(TARGET).elf $^ $(CFLAGS) $(IMPL_THREAD_FLAGS)


%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(IMPL_THREAD_FLAGS)

.PHONY: clean

clean:
	rm -rf *.o *.elf $(OBJS)