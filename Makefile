CFLAGS += -I. -O0 -std=c99
LFLAGS += -lm

nanchecker: src/nanchecker.c cmdline.c
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

test: nanchecker
	./nanchecker

cmdline.c: src/cmdline.ggo
	gengetopt --input=src/cmdline.ggo --include-getopt

help:
	cat README.md

clean:
	rm ./nanchecker

.PHONY: help test nanchecker
