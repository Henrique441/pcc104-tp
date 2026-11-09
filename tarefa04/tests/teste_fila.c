
#include <stdio.h>
#include "fila_prioridade.h"

int main(void) {
    FilaPrioridade *fila = criarFilaPrioridade(5);

    if (fila == NULL) {
        return 1;
    }

    inserir(fila, 1, 10);
    inserir(fila, 2, 5);
    inserir(fila, 3, 15);
    inserir(fila, 4, 8);

    diminuirPrioridade(fila, 3, 2);

    while (!filaVazia(fila)) {
        printf("%d\n", extrairMinimo(fila));
    }

    destruirFilaPrioridade(fila);

    return 0;
}
