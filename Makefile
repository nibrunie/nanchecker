CFLAGS += -O0 -mavx -std=c99

nanchecker: src/nanchecker.c
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $^

test: nanchecker
	./nanchecker

help:
	cat README.md

clean:
	rm ./nanchecker

.PHONY: help test nanchecker
