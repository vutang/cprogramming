LIBS = -pthread

thr_pool_main.elf: thr_pool_main.c thr_pool.c thr_pool.h
	gcc -o $@ $^ $(LIBS)

.PHONY: clean

clean:
	rm -rf *.elf