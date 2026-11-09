
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -Iinclude

TARGET = tarefa04.exe

SRC = src/main.c \
      src/grafo.c \
      src/dijkstra.c \
      src/fila_prioridade.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
