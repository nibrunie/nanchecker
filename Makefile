CFLAGS += -O0 -std=c99
LFLAGS += -lm

nanchecker: src/nanchecker.c
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

test: nanchecker
	./nanchecker

help:
	cat README.md

clean:
	rm ./nanchecker

.PHONY: help test nanchecker
