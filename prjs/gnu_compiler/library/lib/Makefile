LIB=libvux
LIB_OBJS=libvux.o

all: build

build: $(LIB).a $(LIB).so

$(LIB).a:$(LIB_OBJS) 
	$(AR) rcs $@ $(LIB_OBJS)

$(LIB).so:$(LIB_OBJS) 
	$(CC) -shared -o $@.1.0.1 $(LIB_OBJS)
	ln -s $@.1.0.1 $@.1
	ln -s $@.1.0.1 $@

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -rf *.o *.so *.so.* *.a