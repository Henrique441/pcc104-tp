
#include <stdio.h>
#include "grafo.h"

int main(void) {
    Grafo *grafo = lerGrafo();

    if (grafo == NULL) {
        fprintf(stderr, "Erro ao carregar o grafo.\n");
        return 1;
    }

    printf("Grafo carregado com sucesso!\n");
    printf("Vertices: %d\n", grafo->numVertices);
    printf("Arestas: %d\n", grafo->numArestas);

    destruirGrafo(grafo);

    return 0;
}
