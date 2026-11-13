
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "busca_completa.h"

//estrutura para controlar o tempo de execução da busca completa
typedef struct {
    clock_t inicio;
    double tempoMaximo;
    int interrompida;
} ControleBusca;

//função auxiliar para verificar se o tempo máximo de execução foi atingido
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

//função recursiva para explorar todos os caminhos possíveis entre a origem e o destino
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

    //se o vértice atual é o destino, atualizamos o melhor caminho encontrado
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
    } else {//se o vértice atual não é o destino, exploramos os vizinhos
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

//função principal para realizar a busca completa entre a origem e o destino
ResultadoBuscaCompleta *buscaCompleta( const Grafo *grafo, int origem, int destino, double tempoMaximo) {
    if (grafo == NULL || origem < 1 || origem > grafo->numVertices || destino < 1 || destino > grafo->numVertices || tempoMaximo < 0) {
        return NULL;
    }

    int n = grafo->numVertices;

    ResultadoBuscaCompleta *resultado = calloc(1, sizeof(ResultadoBuscaCompleta));

    if (resultado == NULL)
        return NULL;

    resultado->origem = origem;
    resultado->destino = destino;
    resultado->numVertices = n;
    resultado->melhorCusto = INT64_MAX;

    resultado->melhorCaminho = malloc((n + 1) * sizeof(int));

    int *visitado = calloc(n + 1, sizeof(int));
    int *caminhoAtual = malloc((n + 1) * sizeof(int));

    //verifica se a alocação de memória foi bem-sucedida
    if (resultado->melhorCaminho == NULL || visitado == NULL || caminhoAtual == NULL) {

        free(visitado);
        free(caminhoAtual);
        destruirResultadoBuscaCompleta(resultado);
        return NULL;
    }

    //inicializa o controle de tempo da busca completa
    ControleBusca controle = {
        .inicio = clock(),
        .tempoMaximo = tempoMaximo,
        .interrompida = 0
    };

    //inicia a exploração dos caminhos a partir do vértice de origem
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

void imprimirResultadoBuscaCompleta(const ResultadoBuscaCompleta *resultado) {
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

void destruirResultadoBuscaCompleta(ResultadoBuscaCompleta *resultado) {
    if (resultado == NULL)
        return;

    free(resultado->melhorCaminho);
    free(resultado);
}
