
#include <stdio.h>
#include <stdlib.h>
#include "fila_prioridade.h"

//Funções auxiliares para manipulação do heap
static void trocarElementos(FilaPrioridade *fila, int i, int j) {
    ElementoHeap temp = fila->elementos[i];

    fila->elementos[i] = fila->elementos[j];
    fila->elementos[j] = temp;

    fila->posicao[fila->elementos[i].vertice] = i;
    fila->posicao[fila->elementos[j].vertice] = j;
}

//Função auxiliar para subir um elemento na heap
static void subirHeap(FilaPrioridade *fila, int i) {
    while (i > 0) {
        int pai = (i - 1) / 2;

        if (fila->elementos[pai].prioridade <= fila->elementos[i].prioridade) {// Se o pai tem prioridade menor ou igual, a heap está correta
            break;
        }

        trocarElementos(fila, i, pai);
        i = pai;
    }
}

static void descerHeap(FilaPrioridade *fila, int i) {//Função auxiliar para descer um elemento na heap
    while (1) {
        int menor = i;
        int esquerdo = 2 * i + 1;
        int direito = 2 * i + 2;

        // Se o filho esquerdo existe e tem prioridade menor que o elemento atual, atualizamos o menor
        if (esquerdo < fila->tamanho && fila->elementos[esquerdo].prioridade < fila->elementos[menor].prioridade) {
            menor = esquerdo;
        }
        // Se o filho direito existe e tem prioridade menor que o menor atual, atualizamos o menor
        if (direito < fila->tamanho && fila->elementos[direito].prioridade < fila->elementos[menor].prioridade) {
            menor = direito;
        }

        if (menor == i) {
            break;
        }

        trocarElementos(fila, i, menor);
        i = menor;
    }
}

//funções da fila de prioridade
FilaPrioridade *criarFilaPrioridade(int capacidade) {
    if (capacidade <= 0) {
        return NULL;
    }

    FilaPrioridade *fila = malloc(sizeof(FilaPrioridade));

    if (fila == NULL) {
        return NULL;
    }

    fila->elementos = malloc(capacidade * sizeof(ElementoHeap));
    fila->posicao = malloc((capacidade + 1) * sizeof(int));
    //+1 para lidar com o caso em que o vértice é 1, já que os vértices começam em 1 e não em 0
    if (fila->elementos == NULL || fila->posicao == NULL) {
        free(fila->elementos);
        free(fila->posicao);
        free(fila);
        return NULL;
    }

    fila->tamanho = 0;
    fila->capacidade = capacidade;

    for (int i = 0; i <= capacidade; i++) {
        fila->posicao[i] = -1;
    }

    return fila;
}

//função para inserir um elemento na fila de prioridade
int inserir(FilaPrioridade *fila, int vertice, int64_t prioridade) {
    if (fila == NULL ||
        vertice < 1 ||
        vertice > fila->capacidade ||
        fila->tamanho >= fila->capacidade ||
        fila->posicao[vertice] != -1) {
        return 0;
    }

    int i = fila->tamanho;
    fila->tamanho++;

    fila->elementos[i].vertice = vertice;
    fila->elementos[i].prioridade = prioridade;
    fila->posicao[vertice] = i;

    subirHeap(fila, i);

    return 1;
}

//função para extrair o elemento com a menor prioridade da fila de prioridade
int extrairMinimo(FilaPrioridade *fila) {
    if (fila == NULL || fila->tamanho == 0) {
        return -1;
    }

    int minimo = fila->elementos[0].vertice;

    fila->posicao[minimo] = -1;
    fila->tamanho--;

    if (fila->tamanho > 0) {
        fila->elementos[0] = fila->elementos[fila->tamanho];

        fila->posicao[fila->elementos[0].vertice] = 0;

        descerHeap(fila, 0);
    }

    return minimo;
}

//função para diminuir a prioridade de um elemento na fila de prioridade
int diminuirPrioridade(FilaPrioridade *fila, int vertice, int64_t novaPrioridade) {
    if (fila == NULL ||
        vertice < 1 ||
        vertice > fila->capacidade) {
        return 0;
    }

    int i = fila->posicao[vertice];

    if (i == -1 ||
        novaPrioridade > fila->elementos[i].prioridade) {
        return 0;
    }

    fila->elementos[i].prioridade = novaPrioridade;

    subirHeap(fila, i);

    return 1;
}

//função para verificar se a fila de prioridade está vazia
int filaVazia(const FilaPrioridade *fila) {
    return fila == NULL || fila->tamanho == 0;
}

//função para destruir a fila de prioridade e liberar a memória alocada
void destruirFilaPrioridade(FilaPrioridade *fila) {
    if (fila == NULL) {
        return;
    }

    free(fila->elementos);
    free(fila->posicao);
    free(fila);
}
