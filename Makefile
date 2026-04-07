CC	= gcc
SRC = main.c
BIN = main

all: $(BIN)

$(BIN): $(SRC)
	$(CC) -o $(BIN) $(SRC)

clean:
	rm $(BIN)
