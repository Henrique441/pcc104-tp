#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "matriz.h"

//cria e aloca o espaço na memoria para a matriz
//O(n^2)
Matriz *createMatriz(int n){
    //confere se nao esta vazio
    if (n <= 0)
        return NULL;

    //aloca a estrutura matriz
    Matriz *m = malloc(sizeof(Matriz));

    //confere se deu erro na alocacao
    if (m == NULL)
        return NULL;

    m->n = n;//atribui o valor de n no n da estrutura

    //aloca o tamaho da matriz
    m->dados = calloc(n, sizeof(int *));

    if (m->dados == NULL) {
        free(m);
        return NULL;
    }

    //O(n^2)
    for (int i = 0; i < n; i++) {
        m->dados[i] = calloc(n, sizeof(int));

        //caso alguma alocacao de errado desaloca os anteriores e retorna null
        if (m->dados[i] == NULL) {
            for (int j = 0; j < i; j++)
                free(m->dados[j]);

            free(m->dados);
            free(m);
            return NULL;
        }
    }

    return m;
}


//Deleta a matriz
//O(n)
void deleteMatriz(Matriz *m){
    //verifica se a matriz e nula
    if (m == NULL)
        return;

    //apaga os vetores mais internos
    for (int i = 0; i < m->n; i++) {
        free(m->dados[i]);
    }

    //deleta o vetor mais externo e a estrutura
    free(m->dados);
    free(m);
}

//printa a matriz
//O(n^2)
void printMatriz(const Matriz *m){
    if (m == NULL)
        return;

    for (int i = 0; i < m->n; i++) {//O(n^2)
        for (int j = 0; j < m->n; j++) {
            printf("%d", m->dados[i][j]);

            if (j < m->n - 1)
                printf(" ");
        }

        printf("\n");
    }
}


//Leitura do arquivo com a matriz
int readMatriz(Matriz *m)//O(n^2)
{
    if (m == NULL)
        return 0;

    for (int i = 0; i < m->n; i++) {
        for (int j = 0; j < m->n; j++) {
            if (scanf("%d", &m->dados[i][j]) != 1)
                return 0;//retorna 0 caso ocorra algum erro na leitura da matriz
        }
    }

    return 1; //retorna 1 caso a leitura ocorra corretamente
}



//cria uma submatriz
//O(n^2)
static Matriz *subMatriz(const Matriz *M, int linha, int coluna, int tamanho){
    Matriz *sub = createMatriz(tamanho); //O(n^2)

    if (sub == NULL)
        return NULL;

    //2 for O(n^2)
    for (int i = 0; i < tamanho; i++) {
        for (int j = 0; j < tamanho; j++) {
            sub->dados[i][j] =
                M->dados[linha + i][coluna + j];
        }
    }

    return sub;
}

//faz soma e duas marizes
//parametro ponteiro da primeitra matriz e ponteiro da segunda matriz
static Matriz *somaMatriz(const Matriz *A, const Matriz *B){
    if (A == NULL || B == NULL || A->n != B->n)
        return NULL;

    Matriz *C = createMatriz(A->n);//O(n^2)

    if (C == NULL)
        return NULL;

    // O(n^2)
    for (int i = 0; i < A->n; i++) {
        for (int j = 0; j < A->n; j++) {
            C->dados[i][j] = A->dados[i][j] + B->dados[i][j];
        }
    }

    return C;
}

//junta quatro quadrantes de uma matriz em uma matriz
static Matriz *juntaMatriz(const Matriz *C11, const Matriz *C12, const Matriz *C21, const Matriz *C22) {
    
    //testa nulo
    if (C11 == NULL || C12 == NULL || C21 == NULL || C22 == NULL)
        return NULL;

    int metade = C11->n;

    Matriz *C = createMatriz(2 * metade);

    if (C == NULL)
        return NULL;

    for (int i = 0; i < metade; i++) {
        for (int j = 0; j < metade; j++) {
            C->dados[i][j] = C11->dados[i][j];
            C->dados[i][j + metade] = C12->dados[i][j];
            C->dados[i + metade][j] = C21->dados[i][j];
            C->dados[i + metade][j + metade] = C22->dados[i][j];
        }
    }

    return C;
}

//fucao que faz a recursao da multiplicacao de matrizes
//parametros ponteiro da matriz A e ponteiro da matriz B
static Matriz *multiplicaRecursivo(const Matriz *A, const Matriz *B){
    if (A == NULL || B == NULL || A->n != B->n)
        return NULL;

    int n = A->n;

    //Caso base
    if (n == 1) { //T(1) = 1
        Matriz *C = createMatriz(1);

        if (C != NULL)
            C->dados[0][0] = A->dados[0][0] * B->dados[0][0];

        return C;
    }

    int m = n / 2;

    //divide as matrizes em quatro blocos
    Matriz *a[4] = { //t(n) = O(n^2)
        subMatriz(A, 0, 0, m),
        subMatriz(A, 0, m, m),
        subMatriz(A, m, 0, m),
        subMatriz(A, m, m, m)
    };

    Matriz *b[4] = {
        subMatriz(B, 0, 0, m),
        subMatriz(B, 0, m, m),
        subMatriz(B, m, 0, m),
        subMatriz(B, m, m, m)
    };

    Matriz *p[8] = {NULL};
    Matriz *c[4] = {NULL};
    Matriz *C = NULL;

    //confere se alguma alocacao falhou, se falhou vai e libera as alocacoes ja feitas
    for (int i = 0; i < 4; i++) {
        if (a[i] == NULL || b[i] == NULL)
            goto cleanup;
    }

    //faz oito multiplicacoes recursivas
    p[0] = multiplicaRecursivo(a[0], b[0]);
    p[1] = multiplicaRecursivo(a[1], b[2]);

    p[2] = multiplicaRecursivo(a[0], b[1]);
    p[3] = multiplicaRecursivo(a[1], b[3]);

    p[4] = multiplicaRecursivo(a[2], b[0]);
    p[5] = multiplicaRecursivo(a[3], b[2]);

    p[6] = multiplicaRecursivo(a[2], b[1]);
    p[7] = multiplicaRecursivo(a[3], b[3]);

    //confere se deu errado, caso sim limpa a memoria
    for (int i = 0; i < 8; i++) {
        if (p[i] == NULL)
            goto cleanup;
    }

    //faz a soma dos resultados
    c[0] = somaMatriz(p[0], p[1]);
    c[1] = somaMatriz(p[2], p[3]);
    c[2] = somaMatriz(p[4], p[5]);
    c[3] = somaMatriz(p[6], p[7]);

    //junta os quadrantes
    C = juntaMatriz(c[0], c[1], c[2], c[3]);

cleanup:
    //libera todas as matrizes temporarias
    for (int i = 0; i < 4; i++) {
        deleteMatriz(a[i]);//O(i)
        deleteMatriz(b[i]);
        deleteMatriz(c[i]);
    }

    for (int i = 0; i < 8; i++)
        deleteMatriz(p[i]);

    return C;
}

// -- ANALISE DE COMPLEXIDADE --
//Tem 8 funcoes recursivas
//divide a matriz em 2 submatrizes de n/2
//a complexidade local é de O(n^2)
//fucao de complexidade => T(n) = 8(n/2) + O(n^2)

//funcao de multiplicacao de matriz
Matriz *multiplicaMatriz(const Matriz *A, const Matriz *B)
{
    //verifica se as matrizes existem e possuem o mesmo tamanho
    if (A == NULL || B == NULL || A->n != B->n || A->n <= 0)
        return NULL;

    int n = A->n;
    int tamanho = 1;

    //encontra a menor potencia de 2 maior ou igual a n
    while (tamanho < n) {// t(n) = O(log(n)) igual da prova
        if (tamanho > INT_MAX / 2)//confere se pode multiplicar tamanho por 2 para nao causar overflow
            return NULL;

        tamanho *= 2;
    }

    //cria matrizes auxiliares preenchidas com zeros
    Matriz *Ap = createMatriz(tamanho);
    Matriz *Bp = createMatriz(tamanho);

    //verifica se a alocacao funcionou
    if (Ap == NULL || Bp == NULL) {
        deleteMatriz(Ap);
        deleteMatriz(Bp);
        return NULL;
    }

    //copia os elementos originais para as matrizes auxiliares
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            Ap->dados[i][j] = A->dados[i][j];
            Bp->dados[i][j] = B->dados[i][j];
        }
    }

    // Multiplica utilizando divisao e conquista
    Matriz *Cp = multiplicaRecursivo(Ap, Bp);

    //Verifica se a multiplicacao funcionou
    if (Cp == NULL) {
        deleteMatriz(Ap);
        deleteMatriz(Bp);
        return NULL;
    }

    //Cria a matriz resultado com a ordem original
    Matriz *C = createMatriz(n);

    if (C == NULL) {
        deleteMatriz(Ap);
        deleteMatriz(Bp);
        deleteMatriz(Cp);
        return NULL;
    }

    //copia apenas os elementos correspondentes a ordem original
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C->dados[i][j] = Cp->dados[i][j];
        }
    }

    //libera as matrizes auxiliares
    deleteMatriz(Ap);
    deleteMatriz(Bp);
    deleteMatriz(Cp);

    
    return C; //Retorna o resultado
}










