
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -Iinclude

SRC = src/main.c src/grafo.c src/dijkstra.c
OBJ = $(SRC:.c=.o)
TARGET = tarefa02.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
