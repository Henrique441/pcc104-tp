
CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11 -Iinclude

TARGET = tarefa05

SRC = src/main.c src/grafo.c src/busca_completa.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
