
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <limits.h>
#include "grafo.h"
#include "dijkstra.h"

//Main

int main(int argc, char *argv[]) {
    //Confere se a quantidade de argumentos são validas
    if (argc < 1 || argc > 3) {
        fprintf(stderr, "Uso: %s [origem] [destino]\n", argv[0]);
        return 1;
    }

    int origem = 1;
    int destino = -1;

    char *fim;
    long valor;

    //Caso o usuario forneça a origem confere se são validos
    if (argc >= 2) {
        errno = 0;
        valor = strtol(argv[1], &fim, 10);

        //Confere se o valor da origem é valida
        if (errno != 0 || *fim != '\0' || valor < 1 || valor > INT_MAX) {
            fprintf(stderr, "Origem invalida.\n");
            return 1;
        }

        origem = (int)valor;
    }

    //Caso o usuario forneça o destino confere se são validos
    if (argc == 3) {
        errno = 0;
        valor = strtol(argv[2], &fim, 10);

        if (errno != 0 || *fim != '\0' || valor < 1 || valor > INT_MAX) {
            fprintf(stderr, "Destino invalido.\n");
            return 1;
        }

        destino = (int)valor;
    }

    Grafo *grafo = lerGrafo();

    if (grafo == NULL) {
        fprintf(stderr, "Erro ao carregar o grafo.\n");
        return 1;
    }

    if (origem > grafo->numVertices || (destino != -1 && destino > grafo->numVertices)) {
        fprintf(stderr, "Vertice fora dos limites do grafo.\n");
        destruirGrafo(grafo);
        return 1;
    }

    //Executa o algoritmo de Dijkstra
    ResultadoDijkstra *resultado = dijkstra(grafo, origem);

    if (resultado == NULL) {
        fprintf(stderr, "Erro ao executar Dijkstra.\n");
        destruirGrafo(grafo);
        return 1;
    }

    //Confere se o destino esta correto e printa o resultado
    
    if (destino != -1) {
        if (resultado->distancia[destino] == INT64_MAX) {
            printf("Nao existe caminho de %d ate %d.\n",
                origem, destino);
        } else {
            printf("Distancia minima de %d ate %d: %" PRId64 "\n",
                origem, destino, resultado->distancia[destino]);

            printf("Caminho: ");
            imprimirCaminho(resultado, destino);
        }
    } else {
        imprimirTodosCaminhos(resultado);
    }


    destruirResultadoDijkstra(resultado);
    destruirGrafo(grafo);

    return 0;
}
