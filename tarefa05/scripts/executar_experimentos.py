
import argparse
import csv
import re
import subprocess
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


def obter_valor(texto, campo):
    padrao = rf"^{re.escape(campo)}:\s*(.+)$"
    resultado = re.search(padrao, texto, re.MULTILINE)

    if resultado is None:
        raise ValueError(f"Campo nao encontrado: {campo}")

    return resultado.group(1).strip()


def ler_cabecalho_dimacs(caminho):
    with open(caminho, "r", encoding="utf-8") as arquivo:
        for linha in arquivo:
            partes = linha.split()

            if partes and partes[0] == "p":
                return int(partes[2]), int(partes[3])

    raise ValueError("Cabecalho DIMACS nao encontrado")


def executar_busca(executavel, grafo, origem, destino, limite):
    with open(grafo, "r", encoding="utf-8") as entrada:
        processo = subprocess.run(
            [
                str(executavel),
                str(origem),
                str(destino),
                str(limite)
            ],
            stdin=entrada,
            capture_output=True,
            text=True,
            timeout=max(30, limite * 10 + 10)
        )

    if processo.returncode != 0:
        raise RuntimeError(processo.stderr)

    saida = processo.stdout

    custo = obter_valor(saida, "Melhor custo") \
        if "Melhor custo:" in saida else None

    return {
        "tempo": float(obter_valor(saida, "Tempo de execucao").split()[0]),
        "caminhos": int(obter_valor(saida, "Caminhos explorados")),
        "chamadas": int(obter_valor(saida, "Chamadas recursivas")),
        "melhor_custo": int(custo) if custo is not None else None,
        "concluida": obter_valor(saida, "Busca concluida") == "Sim"
    }


def executar_experimentos(
    executavel,
    pasta_grafos,
    pasta_resultados,
    tamanhos,
    origem,
    destino,
    limite,
    repeticoes
):
    pasta_resultados.mkdir(parents=True, exist_ok=True)

    registros = []

    for tamanho in tamanhos:
        grafo = pasta_grafos / f"grafo_{tamanho}.gr"

        if not grafo.exists():
            print(f"Arquivo nao encontrado: {grafo}")
            continue

        vertices, arestas = ler_cabecalho_dimacs(grafo)

        for execucao in range(1, repeticoes + 1):
            print(
                f"Vertices: {vertices} | "
                f"Execucao: {execucao}/{repeticoes}"
            )

            try:
                resultado = executar_busca(
                    executavel,
                    grafo,
                    origem,
                    destino,
                    limite
                )

                registro = {
                    "vertices": vertices,
                    "arestas": arestas,
                    "execucao": execucao,
                    **resultado
                }

                registros.append(registro)

                print(
                    f"Tempo: {resultado['tempo']:.6f}s | "
                    f"Caminhos: {resultado['caminhos']} | "
                    f"Concluida: {resultado['concluida']}"
                )

            except (RuntimeError, subprocess.TimeoutExpired, ValueError) as erro:
                print(f"Erro: {erro}")

    if not registros:
        raise RuntimeError("Nenhum experimento concluido")

    df = pd.DataFrame(registros)

    df.to_csv(
        pasta_resultados / "resultados_completos.csv",
        index=False
    )

    resumo = (
        df.groupby(["vertices", "arestas"], as_index=False)
        .agg(
            tempo_medio=("tempo", "mean"),
            tempo_desvio=("tempo", "std"),
            caminhos_medios=("caminhos", "mean"),
            chamadas_medias=("chamadas", "mean"),
            taxa_conclusao=("concluida", "mean")
        )
    )

    resumo.to_csv(
        pasta_resultados / "resumo_experimentos.csv",
        index=False
    )

    return df, resumo


def gerar_graficos(resumo, pasta_resultados):
    metricas = [
        ("tempo_medio", "Tempo medio (s)", "tempo.png"),
        ("caminhos_medios", "Caminhos explorados", "caminhos.png"),
        ("chamadas_medias", "Chamadas recursivas", "chamadas.png")
    ]

    for coluna, rotulo, arquivo in metricas:
        plt.figure(figsize=(8, 5))

        plt.plot(
            resumo["vertices"],
            resumo[coluna],
            marker="o"
        )

        plt.xlabel("Numero de vertices")
        plt.ylabel(rotulo)
        plt.title(f"{rotulo} por tamanho do grafo")
        plt.grid(True, alpha=0.3)
        plt.tight_layout()

        plt.savefig(
            pasta_resultados / arquivo,
            dpi=200
        )

        plt.close()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--executavel",
        default="./tarefa05.exe"
    )

    parser.add_argument(
        "--grafos",
        default="tests/instancias_roma"
    )

    parser.add_argument(
        "--saida",
        default="resultados"
    )

    parser.add_argument("--origem", type=int, default=1)
    parser.add_argument("--destino", type=int, default=5)
    parser.add_argument("--limite", type=float, default=60)
    parser.add_argument("--repeticoes", type=int, default=5)

    parser.add_argument(
        "--tamanhos",
        type=int,
        nargs="+",
        default=[5, 10, 15, 20, 25, 30, 35, 40]
    )

    args = parser.parse_args()

    executavel = Path(args.executavel).resolve()
    pasta_grafos = Path(args.grafos)
    pasta_resultados = Path(args.saida)

    df, resumo = executar_experimentos(
        executavel,
        pasta_grafos,
        pasta_resultados,
        args.tamanhos,
        args.origem,
        args.destino,
        args.limite,
        args.repeticoes
    )

    gerar_graficos(resumo, pasta_resultados)

    print("\nResumo dos experimentos:")
    print(resumo.to_string(index=False))
