include ../Makefile.inc

TARGET = t_fork
LIBDIR = ../lib
OBJS = 	$(LIBDIR)/error_functions.o \
		$(LIBDIR)/get_num.o \
		$(LIBDIR)/curr_time.o


$(TARGET): $(TARGET).o $(OBJS)
	$(CC) -o $(TARGET).elf $^ $(CFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -rf *.o *.elf $(OBJS)