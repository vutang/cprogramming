TAR = echoclient

CC = gcc
LIBS = -pthread
HDRS = csapp.h
OBJS = csapp.o echo.o $(TAR).o

%.o: %.c $(HDRS)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

${TAR}: ${OBJS}
	gcc -o $@ $^ $(LIBS)

all: $(TAR)
	
clean:
	rm ${TAR}	