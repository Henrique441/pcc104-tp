
#include <stdio.h>
#include <stdlib.h>
#include "fila_prioridade.h"

static void trocarElementos(FilaPrioridade *fila, int i, int j) {
    ElementoHeap temp = fila->elementos[i];

    fila->elementos[i] = fila->elementos[j];
    fila->elementos[j] = temp;

    fila->posicao[fila->elementos[i].vertice] = i;
    fila->posicao[fila->elementos[j].vertice] = j;
}

static void subirHeap(FilaPrioridade *fila, int i) {
    while (i > 0) {
        int pai = (i - 1) / 2;

        if (fila->elementos[pai].prioridade <=
            fila->elementos[i].prioridade) {
            break;
        }

        trocarElementos(fila, i, pai);
        i = pai;
    }
}

static void descerHeap(FilaPrioridade *fila, int i) {
    while (1) {
        int menor = i;
        int esquerdo = 2 * i + 1;
        int direito = 2 * i + 2;

        if (esquerdo < fila->tamanho &&
            fila->elementos[esquerdo].prioridade <
            fila->elementos[menor].prioridade) {
            menor = esquerdo;
        }

        if (direito < fila->tamanho &&
            fila->elementos[direito].prioridade <
            fila->elementos[menor].prioridade) {
            menor = direito;
        }

        if (menor == i) {
            break;
        }

        trocarElementos(fila, i, menor);
        i = menor;
    }
}

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

int diminuirPrioridade(FilaPrioridade *fila, int vertice,
                       int64_t novaPrioridade) {
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

int filaVazia(const FilaPrioridade *fila) {
    return fila == NULL || fila->tamanho == 0;
}

void destruirFilaPrioridade(FilaPrioridade *fila) {
    if (fila == NULL) {
        return;
    }

    free(fila->elementos);
    free(fila->posicao);
    free(fila);
}
