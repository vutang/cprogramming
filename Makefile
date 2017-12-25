TAR = echoclient

OBJS = csapp.o echo.o readline.o $(TAR).o
HDRS = csapp.h

LIBS = -pthread

all: $(OBJS)
	gcc -o $(TAR) $^ $(CFLAGS) $(LIBS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

clean: 
	rm *.o