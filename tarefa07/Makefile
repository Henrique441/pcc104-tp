
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -O2 -Iinclude

TARGET = tarefa07

SRC = src/main.c \
      src/grafo.c \
      src/guloso.c

.PHONY: all build test clean

all: build

build:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

test: build
	./$(TARGET) 1 4 60 < tests/contraexemplo.gr

clean:
	rm -f $(TARGET)
