
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -O2 -Iinclude
TARGET = tarefa06

SRC = src/main.c \
      src/grafo.c \
      src/busca_completa.c \
      src/backtracking.c

.PHONY: all build test clean

all: build

build:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

test: build
	./$(TARGET) 1 5 60 0 < tests/entrada01.gr
	./$(TARGET) 1 5 60 1 < tests/entrada01.gr
	./$(TARGET) 1 5 60 2 < tests/entrada01.gr

clean:
	rm -f $(TARGET)
