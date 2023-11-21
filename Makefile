CC = gcc
CFLAGS = -Wall -Wextra -ggdb -std=c11

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, obj/%.o, $(SRC))
LIB = -lm  # For linux

all: build bool

build:
	@mkdir -p obj

bool: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

obj/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(shell ls bool*) obj

run:
	./bool
