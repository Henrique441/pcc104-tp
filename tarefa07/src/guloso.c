
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "guloso.h"

//função principal para executar o algoritmo guloso
ResultadoGuloso *executarGuloso(
    const Grafo *grafo,
    int origem,
    int destino,
    double tempoMaximo
) {
    if (grafo == NULL || origem < 1 || origem > grafo->numVertices || destino < 1 || destino > grafo->numVertices || tempoMaximo < 0) {
        return NULL;
    }

    clock_t inicio = clock();

    int n = grafo->numVertices;

    ResultadoGuloso *resultado = calloc(1, sizeof(ResultadoGuloso));

    if (resultado == NULL)
        return NULL;

    resultado->origem = origem;
    resultado->destino = destino;

    resultado->caminho = malloc((n + 1) * sizeof(int));

    int *visitado = calloc(n + 1, sizeof(int));

    if (resultado->caminho == NULL || visitado == NULL) {

        free(visitado);
        destruirResultadoGuloso(resultado);
        return NULL;
    }

    int atual = origem;
    int64_t custoAtual = 0;

    while (1) {
        double tempoDecorrido = (double)(clock() - inicio) / CLOCKS_PER_SEC;

        if (tempoMaximo > 0 && tempoDecorrido >= tempoMaximo) {
            break;
        }

        visitado[atual] = 1;

        resultado->caminho[resultado->tamanhoCaminho++] = atual;

        resultado->verticesVisitados++;

        if (atual == destino) {
            resultado->encontrouDestino = 1;
            break;
        }

        const Aresta *aresta = grafo->vertices[atual].primeiraAresta;

        const Aresta *melhorAresta = NULL;

        while (aresta != NULL) {
            int vizinho = aresta->destino;

            if (!visitado[vizinho] &&
                (melhorAresta == NULL ||
                 aresta->peso < melhorAresta->peso ||
                 (aresta->peso == melhorAresta->peso &&
                  vizinho < melhorAresta->destino))) {

                melhorAresta = aresta;
            }

            aresta = aresta->proxima;
        }

        if (melhorAresta == NULL)
            break;

        if (custoAtual > INT64_MAX - melhorAresta->peso) {
            break;
        }

        custoAtual += melhorAresta->peso;
        atual = melhorAresta->destino;
    }

    resultado->custo = custoAtual;

    resultado->tempoExecucao = (double)(clock() - inicio) / CLOCKS_PER_SEC;

    free(visitado);

    return resultado;
}

//função para imprimir o resultado do algoritmo guloso
void imprimirResultadoGuloso(const ResultadoGuloso *resultado) {
    if (resultado == NULL)
        return;

    printf("Origem: %d\n", resultado->origem);
    printf("Destino: %d\n", resultado->destino);

    printf("Destino encontrado: %s\n",
        resultado->encontrouDestino ? "Sim" : "Nao");

    printf("Vertices visitados: %d\n",
        resultado->verticesVisitados);

    printf("Custo: %lld\n",
        (long long)resultado->custo);

    printf("Caminho: ");

    for (int i = 0; i < resultado->tamanhoCaminho; i++) {
        printf("%d", resultado->caminho[i]);

        if (i + 1 < resultado->tamanhoCaminho)
            printf(" -> ");
    }

    printf("\n");

    printf("Tempo de execucao: %.6f s\n",
        resultado->tempoExecucao);
}

void destruirResultadoGuloso(ResultadoGuloso *resultado) {
    if (resultado == NULL)
        return;

    free(resultado->caminho);
    free(resultado);
}
