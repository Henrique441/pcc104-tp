
CC = gcc

CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -O2 -Iinclude

TARGET = tarefa09

SRC = src/main.c \
      src/grafo.c \
      src/floyd_warshall.c

.PHONY: all build test clean

all: build

build:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

test: build
	./$(TARGET) < tests/exemplo.gr
	./$(TARGET) 1 4 < tests/exemplo.gr
	./$(TARGET) 4 1 < tests/exemplo.gr

clean:
	rm -f $(TARGET)
