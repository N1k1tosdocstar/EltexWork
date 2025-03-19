CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -lncurses
SRC = file_manager.c
OUT = my_program

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(OUT)

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)
