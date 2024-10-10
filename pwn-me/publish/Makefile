BIN = \
	chal \

all: $(BIN)

clean:
	rm -f $(BIN) *.o

chal: \
		compresser.c
	gcc -o $@ $^ -lz
