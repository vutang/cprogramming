include ../Make.defines

all:	${LIB_OBJS}
	ar rv ${LIBUNPIPC_NAME} $?

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

clean:
	rm -f ${PROGS} ${CLEANFILES}