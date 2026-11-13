
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#include "simulated_annealing.h"
#include "caminho.h"

static unsigned int proximoAleatorio(unsigned int *estado) {
    unsigned int x = *estado;

    if (x == 0)
        x = 2463534242u;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    *estado = x;
    return x;
}

static double aleatorio01(unsigned int *estado) {
    return (double)proximoAleatorio(estado) /
           ((double)UINT32_MAX + 1.0);
}

static double tempoDecorrido(clock_t inicio) {
    return (double)(clock() - inicio) / CLOCKS_PER_SEC;
}

ResultadoSA *executarSA(
    const Grafo *grafo,
    int origem,
    int destino,
    ConfigSA config
) {
    if (grafo == NULL ||
        origem < 1 || origem > grafo->numVertices ||
        destino < 1 || destino > grafo->numVertices ||
        !isfinite(config.temperaturaInicial) ||
        !isfinite(config.temperaturaMinima) ||
        !isfinite(config.taxaResfriamento) ||
        !isfinite(config.tempoMaximo) ||
        config.temperaturaInicial <= 0.0 ||
        config.temperaturaMinima <= 0.0 ||
        config.temperaturaMinima >= config.temperaturaInicial ||
        config.taxaResfriamento <= 0.0 ||
        config.taxaResfriamento >= 1.0 ||
        config.iteracoesPorTemperatura <= 0 ||
        config.tempoMaximo <= 0.0) {
        return NULL;
    }

    clock_t inicio = clock();
    unsigned int semente = config.semente;

    int n = grafo->numVertices;

    ResultadoSA *resultado = calloc(
        1, sizeof(ResultadoSA)
    );

    if (resultado == NULL)
        return NULL;

    Caminho atual = criarCaminho(n);
    Caminho vizinho = criarCaminho(n);

    resultado->melhorCaminho = criarCaminho(n);

    // Verifica a alocacao dos vetores de vertices e pesos.
    if (atual.vertices == NULL ||
        atual.pesos == NULL ||
        vizinho.vertices == NULL ||
        vizinho.pesos == NULL ||
        resultado->melhorCaminho.vertices == NULL ||
        resultado->melhorCaminho.pesos == NULL) {

        liberarCaminho(&atual);
        liberarCaminho(&vizinho);
        destruirResultadoSA(resultado);

        return NULL;
    }

    // Define o limite de chamadas da busca aleatorizada.
    int limiteChamadas;

    if (n > INT_MAX / 100) {
        limiteChamadas = INT_MAX;
    } else {
        limiteChamadas = n * 100;
    }

    // =====================================================
    // 1. CONSTRUCAO DA SOLUCAO INICIAL
    // =====================================================

    EstadoBusca estadoInicial = construirCaminhoAleatorio(
        grafo,
        origem,
        destino,
        &atual,
        &semente,
        limiteChamadas,
        inicio,
        config.tempoMaximo
    );

    if (estadoInicial == BUSCA_ERRO) {
        liberarCaminho(&atual);
        liberarCaminho(&vizinho);
        destruirResultadoSA(resultado);

        return NULL;
    }

    if (estadoInicial != BUSCA_ENCONTROU) {

        // A ausencia de uma solucao inicial nao significa
        // necessariamente que nao exista caminho no grafo.
        if (estadoInicial == BUSCA_LIMITE) {
            fprintf(stderr,
                "Aviso: busca inicial atingiu "
                "o limite de chamadas.\n"
            );
        } else if (estadoInicial == BUSCA_TEMPO) {
            fprintf(stderr,
                "Aviso: tempo maximo atingido "
                "durante a busca inicial.\n"
            );
        } else if (estadoInicial == BUSCA_NAO_ENCONTROU) {
            fprintf(stderr,
                "Aviso: busca inicial nao encontrou "
                "um caminho valido.\n"
            );
        }

        resultado->tempoExecucao = tempoDecorrido(inicio);

        liberarCaminho(&atual);
        liberarCaminho(&vizinho);

        return resultado;
    }

    resultado->encontrouDestino = 1;

    // Inicialmente, o melhor caminho e o caminho encontrado.
    if (!copiarCaminho(
        &resultado->melhorCaminho,
        &atual
    )) {
        liberarCaminho(&atual);
        liberarCaminho(&vizinho);
        destruirResultadoSA(resultado);

        return NULL;
    }

    // =====================================================
    // 2. INICIALIZACAO DO SIMULATED ANNEALING
    // =====================================================

    double temperatura = config.temperaturaInicial;

    // =====================================================
    // 3. PROCESSO DE OTIMIZACAO
    // =====================================================

    while (temperatura > config.temperaturaMinima &&
           tempoDecorrido(inicio) < config.tempoMaximo) {

        for (int i = 0;
             i < config.iteracoesPorTemperatura;
             i++) {

            if (tempoDecorrido(inicio) >= config.tempoMaximo)
                break;

            resultado->iteracoes++;

            // =============================================
            // 3.1. GERACAO DE UMA SOLUCAO VIZINHA
            // =============================================

            EstadoBusca estadoVizinho = gerarVizinho(
                grafo,
                &atual,
                destino,
                &vizinho,
                &semente,
                limiteChamadas,
                inicio,
                config.tempoMaximo
            );

            if (estadoVizinho == BUSCA_TEMPO) {
                // O tempo limite foi atingido.
                break;
            }

            if (estadoVizinho == BUSCA_ERRO) {
                liberarCaminho(&atual);
                liberarCaminho(&vizinho);
                destruirResultadoSA(resultado);

                return NULL;
            }

            if (estadoVizinho != BUSCA_ENCONTROU) {
                // Nenhum vizinho valido foi produzido
                // nesta tentativa.
                continue;
            }

            // =============================================
            // 3.2. CRITERIO DE ACEITACAO
            // =============================================

            int aceitar = 0;

            if (vizinho.custo <= atual.custo) {

                // Solucoes melhores ou iguais
                // sao sempre aceitas.
                aceitar = 1;

            } else {

                // Calcula a diferenca em double para
                // evitar overflow na subtracao de int64_t.
                double diferenca =
                    (double)vizinho.custo -
                    (double)atual.custo;

                double probabilidade = exp(
                    -diferenca / temperatura
                );

                if (aleatorio01(&semente) < probabilidade) {
                    aceitar = 1;
                }
            }

            // =============================================
            // 3.3. ATUALIZACAO DA SOLUCAO ATUAL
            // =============================================

            if (aceitar) {

                if (!copiarCaminho(&atual, &vizinho)) {
                    liberarCaminho(&atual);
                    liberarCaminho(&vizinho);
                    destruirResultadoSA(resultado);

                    return NULL;
                }

                resultado->solucoesAceitas++;
            }

            // =============================================
            // 3.4. ATUALIZACAO DA MELHOR SOLUCAO
            // =============================================

            if (atual.custo < resultado->melhorCaminho.custo) {

                if (!copiarCaminho(
                    &resultado->melhorCaminho,
                    &atual
                )) {
                    liberarCaminho(&atual);
                    liberarCaminho(&vizinho);
                    destruirResultadoSA(resultado);

                    return NULL;
                }

                resultado->solucoesMelhores++;
            }
        }

        // =============================================
        // 3.5. RESFRIAMENTO
        // =============================================

        temperatura *= config.taxaResfriamento;
    }

    // =====================================================
    // 4. FINALIZACAO
    // =====================================================

    resultado->tempoExecucao = tempoDecorrido(inicio);

    liberarCaminho(&atual);
    liberarCaminho(&vizinho);

    return resultado;
}

void imprimirResultadoSA(const ResultadoSA *resultado) {
    if (resultado == NULL)
        return;

    printf("Destino encontrado: %s\n",
           resultado->encontrouDestino ? "Sim" : "Nao");

    printf("Iteracoes: %d\n",
           resultado->iteracoes);

    printf("Solucoes aceitas: %d\n",
           resultado->solucoesAceitas);

    printf("Melhorias: %d\n",
           resultado->solucoesMelhores);

    if (resultado->encontrouDestino) {

        printf("Custo: %" PRId64 "\n",
               resultado->melhorCaminho.custo);

        printf("Caminho: ");

        for (int i = 0;
             i < resultado->melhorCaminho.tamanho;
             i++) {

            if (i > 0)
                printf(" -> ");

            printf("%d",
                   resultado->melhorCaminho.vertices[i]);
        }

        printf("\n");
    }

    printf("Tempo de execucao: %.6f s\n",
           resultado->tempoExecucao);
}

void destruirResultadoSA(ResultadoSA *resultado) {
    if (resultado == NULL)
        return;

    liberarCaminho(&resultado->melhorCaminho);
    free(resultado);
}
