TAR = echoclient

OBJS = csapp.o handler.o readline.o $(TAR).o
HDRS = csapp.h cliSer.h unp.h

LIBS = -pthread

all: $(OBJS)
	gcc -o $(TAR) $^ $(CFLAGS) $(LIBS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

clean: 
	rm *.o