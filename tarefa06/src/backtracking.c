
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "backtracking.h"

//estrutura para controlar o tempo de execução do backtracking
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

    double decorrido =
        (double)(clock() - controle->inicio) / CLOCKS_PER_SEC;

    if (decorrido >= controle->tempoMaximo) {
        controle->interrompida = 1;
        return 1;
    }

    return 0;
}

// Calcula distancias minimas de cada vertice ate o destino.
// Equivale a executar Dijkstra no grafo transposto.
static int64_t *calcularDistancias(
    const Grafo *grafo,
    int destino,
    ControleBusca *controle
) {
    int n = grafo->numVertices;

    int64_t *dist = malloc((n + 1) * sizeof(int64_t));
    int *processado = calloc(n + 1, sizeof(int));
    // processado[v] = 1 se o vertice v ja foi processado
    if (dist == NULL || processado == NULL) {
        free(dist);
        free(processado);
        return NULL;
    }

    for (int i = 0; i <= n; i++)
        dist[i] = INT64_MAX;

    dist[destino] = 0;

    // Executa Dijkstra no grafo transposto.
    for (int i = 1; i <= n; i++) {
        if (tempoEsgotado(controle))
            break;

        int atual = -1;
        //Encontra o vertice nao processado com a menor distancia
        for (int v = 1; v <= n; v++) {
            if (!processado[v] &&
                (atual == -1 || dist[v] < dist[atual])) {
                atual = v;
            }
        }

        if (atual == -1 || dist[atual] == INT64_MAX)
            break;

        processado[atual] = 1;

        //Percorre arestas originais u -> atual.
        for (int u = 1; u <= n; u++) {
            if (tempoEsgotado(controle))
                break;

            const Aresta *aresta =
                grafo->vertices[u].primeiraAresta;

            while (aresta != NULL) {
                if (aresta->destino == atual &&
                    !processado[u] &&
                    dist[atual] <= INT64_MAX - aresta->peso) {

                    int64_t novo =
                        dist[atual] + aresta->peso;

                    if (novo < dist[u])
                        dist[u] = novo;
                }

                aresta = aresta->proxima;
            }
        }
    }

    free(processado);
    return dist;
}

//Função recursiva para explorar todos os caminhos possíveis entre a origem e o destino
static void explorarCaminhos(
    const Grafo *grafo,
    int atual,
    int destino,
    int *visitado,
    int *caminhoAtual,
    int tamanhoCaminho,
    int64_t custoAtual,
    const int64_t *distancias,
    int modo,
    ResultadoBacktracking *resultado,
    ControleBusca *controle
) {
    if (tempoEsgotado(controle))
        return;

    resultado->numChamadasRecursivas++;

    visitado[atual] = 1;
    caminhoAtual[tamanhoCaminho++] = atual;

    if (atual == destino) {
        resultado->numCaminhosExplorados++;

        if (resultado->tamanhoMelhorCaminho == 0 ||
            custoAtual < resultado->melhorCusto) {

            resultado->melhorCusto = custoAtual;
            resultado->tamanhoMelhorCaminho = tamanhoCaminho;

            for (int i = 0; i < tamanhoCaminho; i++)
                resultado->melhorCaminho[i] = caminhoAtual[i];
        }

        visitado[atual] = 0;
        return;
    }

    //Poda 1: custo acumulado.
    if (resultado->tamanhoMelhorCaminho > 0 &&
        custoAtual >= resultado->melhorCusto) {

        resultado->podasCusto++;
        resultado->numPodasRealizadas++;

        visitado[atual] = 0;
        return;
    }

    //Poda 2: distancia minima restante.
    if (modo == 2) {
        int podar = (distancias[atual] == INT64_MAX);

        if (!podar &&
            resultado->tamanhoMelhorCaminho > 0) {

            int64_t restante = distancias[atual];

            podar =
                restante > INT64_MAX - custoAtual ||
                custoAtual + restante >= resultado->melhorCusto;
        }

        if (podar) {
            resultado->podasLimiteInferior++;
            resultado->numPodasRealizadas++;

            visitado[atual] = 0;
            return;
        }
    }

    const Aresta *aresta = grafo->vertices[atual].primeiraAresta;

    //Explora os vizinhos do vértice atual
    while (aresta != NULL) {
        if (tempoEsgotado(controle))
            break;

        int vizinho = aresta->destino;

        if (!visitado[vizinho] &&
            custoAtual <= INT64_MAX - aresta->peso) {

            explorarCaminhos(
                grafo,
                vizinho,
                destino,
                visitado,
                caminhoAtual,
                tamanhoCaminho,
                custoAtual + aresta->peso,
                distancias,
                modo,
                resultado,
                controle
            );
        }

        aresta = aresta->proxima;
    }

    visitado[atual] = 0;
}

//Função principal para executar o backtracking com ou sem poda por limite inferior
ResultadoBacktracking *executarBacktrackingModo(
    const Grafo *grafo,
    int origem,
    int destino,
    double tempoMaximo,
    int modo
) {
    if (grafo == NULL || origem < 1 || origem > grafo->numVertices || destino < 1 || destino > grafo->numVertices || tempoMaximo < 0 || (modo != 1 && modo != 2)) {
        return NULL;
    }

    int n = grafo->numVertices;

    //Aloca memória para o resultado do backtracking
    ResultadoBacktracking *resultado = calloc(1, sizeof(ResultadoBacktracking));

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
        destruirResultadoBacktracking(resultado);
        return NULL;
    }

    //inicializa o controle de tempo da busca completa
    ControleBusca controle = {
        .inicio = clock(),
        .tempoMaximo = tempoMaximo,
        .interrompida = 0
    };

    int64_t *distancias = NULL;

    if (modo == 2) {
        distancias = calcularDistancias(
            grafo, destino, &controle
        );

        if (distancias == NULL) {
            free(visitado);
            free(caminhoAtual);
            destruirResultadoBacktracking(resultado);
            return NULL;
        }
    }

    if (!controle.interrompida) {
        explorarCaminhos(
            grafo, origem, destino,
            visitado, caminhoAtual,
            0, 0, distancias, modo,
            resultado, &controle
        );
    }

    resultado->buscaConcluida = !controle.interrompida;
    resultado->tempoExecucao = (double)(clock() - controle.inicio) / CLOCKS_PER_SEC;

    free(distancias);
    free(visitado);
    free(caminhoAtual);

    return resultado;
}


//função principal para executar o backtracking sem poda por limite inferior
ResultadoBacktracking *executarBacktracking(
    const Grafo *grafo,
    int origem,
    int destino,
    double tempoMaximo
) {
    return executarBacktrackingModo(grafo, origem, destino, tempoMaximo, 1);
}

//função principal para executar o backtracking com poda por limite inferior
void imprimirResultadoBacktracking(const ResultadoBacktracking *resultado) {
    if (resultado == NULL)
        return;

    printf("Origem: %d\n", resultado->origem);
    printf("Destino: %d\n", resultado->destino);

    printf("Caminhos explorados: %llu\n",
        (unsigned long long)resultado->numCaminhosExplorados);

    printf("Chamadas recursivas: %llu\n",
        (unsigned long long)resultado->numChamadasRecursivas);

    printf("Podas realizadas: %llu\n",
        (unsigned long long)resultado->numPodasRealizadas);

    printf("Podas por custo: %llu\n",
        (unsigned long long)resultado->podasCusto);

    printf("Podas por limite inferior: %llu\n",
        (unsigned long long)resultado->podasLimiteInferior);

    printf("Tempo de execucao: %.6f s\n",
        resultado->tempoExecucao);

    if (resultado->tamanhoMelhorCaminho == 0) {
        printf("Nenhum caminho encontrado.\n");
    } else {
        printf("Melhor custo: %lld\n",
            (long long)resultado->melhorCusto);

        printf("Melhor caminho: ");

        for (int i = 0; i < resultado->tamanhoMelhorCaminho; i++) {
            printf("%d", resultado->melhorCaminho[i]);

            if (i + 1 < resultado->tamanhoMelhorCaminho)
                printf(" -> ");
        }

        printf("\n");
    }

    printf("Busca concluida: %s\n",
        resultado->buscaConcluida ? "Sim" : "Nao");
}

//função para liberar a memória alocada para o resultado do backtracking
void destruirResultadoBacktracking(ResultadoBacktracking *resultado) {
    if (resultado == NULL)
        return;

    free(resultado->melhorCaminho);
    free(resultado);
}
