TAR = main
LDLIBS = -lvux

SRC = main.c

all: build

# -Llib/: -L{path to file containing library} -l${library name}
# to specify libraries
build:
	gcc -I $(PWD)/../lib/ -L$(PWD)/../lib/ -o $(TAR) $(SRC) $(LDLIBS)

clean:
	rm $(TAR) *.o
