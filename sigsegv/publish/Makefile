BIN = \
	mod

all: $(BIN)

clean:
	rm -f $(BIN) *.o

mod: \
		mod.c
	gcc -o $@ $^ -lcurses
