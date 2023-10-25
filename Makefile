CC = gcc
CFLAGS = -Wall -Wextra -ggdb -std=c11

SRC = $(wildcard *.c)
OBJ = $(patsubst %.c, obj/%.o, $(SRC))

all: build bool

build:
	@mkdir -p obj

bool: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

obj/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf *.exe obj

run:
	./bool.exe
