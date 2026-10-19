
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <ordem>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);

    if (n <= 0) {
        fprintf(stderr, "Ordem invalida.\n");
        return 1;
    }

    printf("%d\n", n);

    // Matriz A
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", (i + j) % 10);
        }
        printf("\n");
    }

    // Matriz B
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", (i * j) % 10);
        }
        printf("\n");
    }

    return 0;
}
