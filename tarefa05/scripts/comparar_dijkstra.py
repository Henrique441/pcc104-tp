
import argparse
import re
import subprocess
from pathlib import Path

import pandas as pd


def executar_programa(executavel, argumentos, grafo, timeout):
    with open(grafo, "r", encoding="utf-8") as entrada:
        processo = subprocess.run(
            [str(executavel), *map(str, argumentos)],
            stdin=entrada,
            capture_output=True,
            text=True,
            timeout=timeout
        )

    if processo.returncode != 0:
        raise RuntimeError(processo.stderr)

    return processo.stdout


def extrair_busca_completa(saida):
    custo = re.search(
        r"^Melhor custo:\s*(\d+)",
        saida,
        re.MULTILINE
    )

    concluida = re.search(
        r"^Busca concluida:\s*(Sim|Nao)",
        saida,
        re.MULTILINE
    )

    return {
        "custo_busca": int(custo.group(1)) if custo else None,
        "concluida": (
            concluida.group(1) == "Sim"
            if concluida else False
        )
    }


def extrair_dijkstra(saida, origem, destino):
    padrao = (
        rf"^\[{origem},{destino}\]"
        rf"\((INF|\d+)\)"
    )

    resultado = re.search(padrao, saida, re.MULTILINE)

    if resultado is None:
        raise ValueError("Destino nao encontrado na saida de Dijkstra")

    custo = resultado.group(1)

    return None if custo == "INF" else int(custo)


def comparar(
    pasta_grafos,
    executavel_busca,
    executavel_dijkstra,
    tamanhos,
    origem,
    destino,
    limite
):
    registros = []

    for tamanho in tamanhos:
        grafo = pasta_grafos / f"grafo_{tamanho}.gr"

        if not grafo.exists():
            continue

        print(f"Comparando grafo com {tamanho} vertices")

        saida_busca = executar_programa(
            executavel_busca,
            [origem, destino, limite],
            grafo,
            timeout=max(30, limite * 10 + 10)
        )

        saida_dijkstra = executar_programa(
            executavel_dijkstra,
            [origem],
            grafo,
            timeout=120
        )

        resultado_busca = extrair_busca_completa(saida_busca)

        custo_dijkstra = extrair_dijkstra(
            saida_dijkstra,
            origem,
            destino
        )

        custo_busca = resultado_busca["custo_busca"]
        concluida = resultado_busca["concluida"]

        coincide = custo_busca == custo_dijkstra

        registros.append({
            "vertices": tamanho,
            "custo_busca": custo_busca,
            "custo_dijkstra": custo_dijkstra,
            "busca_concluida": concluida,
            "custos_coincidem": coincide
        })

    return pd.DataFrame(registros)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--grafos",
        default="tests/instancias_sinteticas/densidade_50"
    )

    parser.add_argument(
        "--busca",
        default="./tarefa05.exe"
    )

    parser.add_argument(
        "--dijkstra",
        default="../tarefa04/tarefa04.exe"
    )

    parser.add_argument("--origem", type=int, default=1)
    parser.add_argument("--destino", type=int, default=5)
    parser.add_argument("--limite", type=float, default=60)

    parser.add_argument(
        "--tamanhos",
        type=int,
        nargs="+",
        default=[5, 10, 15, 20, 25]
    )

    parser.add_argument(
        "--saida",
        default="resultados/comparacao_dijkstra.csv"
    )

    args = parser.parse_args()

    df = comparar(
        Path(args.grafos),
        Path(args.busca).resolve(),
        Path(args.dijkstra).resolve(),
        args.tamanhos,
        args.origem,
        args.destino,
        args.limite
    )

    saida = Path(args.saida)
    saida.parent.mkdir(parents=True, exist_ok=True)

    df.to_csv(saida, index=False)

    print("\nResultados:")
    print(df.to_string(index=False))
