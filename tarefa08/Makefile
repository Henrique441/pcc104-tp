
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -O2 -Iinclude

TARGET = tarefa08.exe

SRC = src/main.c \
      src/grafo.c \
      src/caminho.c \
      src/simulated_annealing.c

.PHONY: all build test clean

all: build

build:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) -lm

test: build
	./$(TARGET) 1 4 5 12345 < tests/contraexemplo.gr

clean:
	rm -f $(TARGET)
