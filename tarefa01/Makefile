CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -Iinclude

TARGET = tarefa01.exe

SRC = src/main.c src/matriz.c

all: $(TARGET)

$(TARGET): $(SRC) include/matriz.h
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

test: $(TARGET)
	@for i in 01 02 03 04; do \
				./$(TARGET) < tests/entrada$$i.txt | tr -d '\r' > resultado.txt; \
		if diff -q tests/saida$$i.txt resultado.txt > /dev/null; then \
			echo "Teste $$i: PASSOU"; \
		else \
			echo "Teste $$i: FALHOU"; \
			diff -u tests/saida$$i.txt resultado.txt; \
			exit 1; \
		fi; \
	done
	@rm -f resultado.txt

clean:
	rm -f $(TARGET) matriz.o resultado.txt

.PHONY: all test clean