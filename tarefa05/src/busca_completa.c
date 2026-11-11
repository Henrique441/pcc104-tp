
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "busca_completa.h"

typedef struct {
    clock_t inicio;
    double tempoMaximo;
    int interrompida;
} ControleBusca;

static int tempoEsgotado(ControleBusca *controle) {
    if (controle->interrompida)
        return 1;

    if (controle->tempoMaximo <= 0)
        return 0;

    double tempoDecorrido =
        (double)(clock() - controle->inicio) / CLOCKS_PER_SEC;

    if (tempoDecorrido >= controle->tempoMaximo) {
        controle->interrompida = 1;
        return 1;
    }

    return 0;
}

static void explorarCaminhos(
    const Grafo *grafo,
    int atual,
    int destino,
    int *visitado,
    int *caminhoAtual,
    int tamanhoCaminho,
    int64_t custoAtual,
    ResultadoBuscaCompleta *resultado,
    ControleBusca *controle
) {
    if (tempoEsgotado(controle))
        return;

    resultado->numChamadasRecursivas++;

    visitado[atual] = 1;
    caminhoAtual[tamanhoCaminho] = atual;
    tamanhoCaminho++;

    if (atual == destino) {
        resultado->numCaminhosExplorados++;

        if (resultado->tamanhoMelhorCaminho == 0 ||
            custoAtual < resultado->melhorCusto) {

            resultado->melhorCusto = custoAtual;
            resultado->tamanhoMelhorCaminho = tamanhoCaminho;

            for (int i = 0; i < tamanhoCaminho; i++) {
                resultado->melhorCaminho[i] = caminhoAtual[i];
            }
        }
    } else {
        const Aresta *aresta =
            grafo->vertices[atual].primeiraAresta;

        while (aresta != NULL) {
            if (tempoEsgotado(controle))
                break;

            int vizinho = aresta->destino;

            if (!visitado[vizinho]) {
                if (aresta->peso >= 0 &&
                    custoAtual <= INT64_MAX - aresta->peso) {

                    explorarCaminhos(
                        grafo,
                        vizinho,
                        destino,
                        visitado,
                        caminhoAtual,
                        tamanhoCaminho,
                        custoAtual + aresta->peso,
                        resultado,
                        controle
                    );
                }
            }

            aresta = aresta->proxima;
        }
    }

    visitado[atual] = 0;
}

ResultadoBuscaCompleta *buscaCompleta(
    const Grafo *grafo,
    int origem,
    int destino,
    double tempoMaximo
) {
    if (grafo == NULL ||
        origem < 1 || origem > grafo->numVertices ||
        destino < 1 || destino > grafo->numVertices ||
        tempoMaximo < 0) {
        return NULL;
    }

    int n = grafo->numVertices;

    ResultadoBuscaCompleta *resultado =
        calloc(1, sizeof(ResultadoBuscaCompleta));

    if (resultado == NULL)
        return NULL;

    resultado->origem = origem;
    resultado->destino = destino;
    resultado->numVertices = n;
    resultado->melhorCusto = INT64_MAX;

    resultado->melhorCaminho =
        malloc((n + 1) * sizeof(int));

    int *visitado = calloc(n + 1, sizeof(int));
    int *caminhoAtual = malloc((n + 1) * sizeof(int));

    if (resultado->melhorCaminho == NULL ||
        visitado == NULL ||
        caminhoAtual == NULL) {

        free(visitado);
        free(caminhoAtual);
        destruirResultadoBuscaCompleta(resultado);
        return NULL;
    }

    ControleBusca controle = {
        .inicio = clock(),
        .tempoMaximo = tempoMaximo,
        .interrompida = 0
    };

    explorarCaminhos(
        grafo,
        origem,
        destino,
        visitado,
        caminhoAtual,
        0,
        0,
        resultado,
        &controle
    );

    resultado->buscaConcluida = !controle.interrompida;

    free(visitado);
    free(caminhoAtual);

    return resultado;
}

void imprimirResultadoBuscaCompleta(
    const ResultadoBuscaCompleta *resultado
) {
    if (resultado == NULL)
        return;

    printf("Origem: %d\n", resultado->origem);
    printf("Destino: %d\n", resultado->destino);

    printf(
        "Caminhos explorados: %llu\n",
        (unsigned long long)resultado->numCaminhosExplorados
    );

    printf(
        "Chamadas recursivas: %llu\n",
        (unsigned long long)resultado->numChamadasRecursivas
    );

    if (resultado->tamanhoMelhorCaminho == 0) {
        printf("Nenhum caminho encontrado.\n");
    } else {
        printf(
            "Melhor custo: %lld\n",
            (long long)resultado->melhorCusto
        );

        printf("Melhor caminho: ");

        for (int i = 0; i < resultado->tamanhoMelhorCaminho; i++) {
            printf("%d", resultado->melhorCaminho[i]);

            if (i < resultado->tamanhoMelhorCaminho - 1)
                printf(" -> ");
        }

        printf("\n");
    }

    printf(
        "Busca concluida: %s\n",
        resultado->buscaConcluida ? "Sim" : "Nao"
    );
}

void destruirResultadoBuscaCompleta(
    ResultadoBuscaCompleta *resultado
) {
    if (resultado == NULL)
        return;

    free(resultado->melhorCaminho);
    free(resultado);
}
