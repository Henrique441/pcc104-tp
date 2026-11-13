
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include <time.h>
#include <limits.h>

#include "simulated_annealing.h"
#include "caminho.h"

//função auxiliar para gerar um número aleatório a partir de um estado
static unsigned int proximoAleatorio(unsigned int *estado) {
    unsigned int x = *estado;

    //se o estado for zero, inicializa com um valor padrão
    if (x == 0)
        x = 2463534242u;

    //faz uma transformação bit a bit para gerar um novo número pseudo-aleatório
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    *estado = x;
    return x;
}

//função auxiliar para gerar um número aleatório entre 0 e 1 a partir de um estado
static double aleatorio01(unsigned int *estado) {
    return (double)proximoAleatorio(estado) /
           ((double)UINT32_MAX + 1.0);
}


//função auxiliar para calcular o tempo decorrido desde um determinado instante
static double tempoDecorrido(clock_t inicio) {
    return (double)(clock() - inicio) / CLOCKS_PER_SEC;
}

//função principal para executar o algoritmo de simulated annealing
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

    ResultadoSA *resultado = calloc(1, sizeof(ResultadoSA) );

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

    
    //Construção de uma solução inicial aleatória
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

        //A ausencia de uma solucao inicial nao significa
        //necessariamente que nao exista caminho no grafo.
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

    //Inicialmente, o melhor caminho e o caminho encontrado.
    if (!copiarCaminho(
        &resultado->melhorCaminho,
        &atual
    )) {
        liberarCaminho(&atual);
        liberarCaminho(&vizinho);
        destruirResultadoSA(resultado);

        return NULL;
    }

    
    //Inicializa a temperatura com o valor definido na configuração.
    double temperatura = config.temperaturaInicial;

    
    //Processa o algoritmo de simulated annealing até que a temperatura mínima seja atingida ou o tempo máximo seja excedido.
    while (temperatura > config.temperaturaMinima &&
           tempoDecorrido(inicio) < config.tempoMaximo) {

        for (int i = 0;
             i < config.iteracoesPorTemperatura;
             i++) {

            if (tempoDecorrido(inicio) >= config.tempoMaximo)
                break;

            resultado->iteracoes++;

            //faz a busca aleatorizada para gerar um vizinho da solução atual

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

            //verifica se houve algum erro durante a busca aleatorizada
            if (estadoVizinho == BUSCA_TEMPO) {
                //tempo limite foi atingido.
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

            //Faz a decisão de aceitar ou rejeitar o vizinho gerado com base na diferença de custo e na temperatura atual.

            int aceitar = 0;

            if (vizinho.custo <= atual.custo) {

                //Solucoes melhores ou iguais sao sempre aceitas.
         
                aceitar = 1;

            } else {

                //Calcula a diferenca em double para
                //evitar overflow na subtracao de int64_t.
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

           
            //Atualiza a solução atual com o vizinho gerado se ele for aceito.
            if (aceitar) {

                if (!copiarCaminho(&atual, &vizinho)) {
                    liberarCaminho(&atual);
                    liberarCaminho(&vizinho);
                    destruirResultadoSA(resultado);

                    return NULL;
                }

                resultado->solucoesAceitas++;
            }

            
            //Atualiza o melhor caminho encontrado se a solução atual for melhor do que o melhor caminho registrado.
            if (atual.custo < resultado->melhorCaminho.custo) {

                if (!copiarCaminho(&resultado->melhorCaminho,&atual)) {
                    liberarCaminho(&atual);
                    liberarCaminho(&vizinho);
                    destruirResultadoSA(resultado);

                    return NULL;
                }

                resultado->solucoesMelhores++;
            }
        }

        //Resfria a temperatura de acordo com a taxa de resfriamento definida na configuração.
        temperatura *= config.taxaResfriamento;
    }


    //Libera a memória alocada para os caminhos temporários e retorna o resultado do algoritmo de simulated annealing.
    resultado->tempoExecucao = tempoDecorrido(inicio);

    liberarCaminho(&atual);
    liberarCaminho(&vizinho);

    return resultado;
}


//função para imprimir o resultado do algoritmo de simulated annealing
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

//função para liberar a memória alocada para o resultado do algoritmo de simulated annealing
void destruirResultadoSA(ResultadoSA *resultado) {
    if (resultado == NULL)
        return;

    liberarCaminho(&resultado->melhorCaminho);
    free(resultado);
}
