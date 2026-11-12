
import argparse
import re
import subprocess
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


def executar(executavel, argumentos, grafo, timeout):
    with open(grafo, "r", encoding="utf-8") as entrada:
        processo = subprocess.run(
            [str(executavel), *map(str, argumentos)],
            stdin=entrada,
            capture_output=True,
            text=True,
            timeout=timeout
        )

    if processo.returncode != 0:
        raise RuntimeError(
            processo.stderr or processo.stdout
        )

    return processo.stdout + "\n" + processo.stderr


def extrair_tempo(saida):
    resultado = re.search(
        r"Tempo de execucao\s*:\s*([0-9]+(?:[.,][0-9]+)?)",
        saida,
        re.IGNORECASE
    )

    if resultado is None:
        raise ValueError(
            "Tempo de execucao nao encontrado.\n"
            f"Saida recebida:\n{repr(saida[:2000])}"
        )

    return float(resultado.group(1).replace(",", "."))


def extrair_busca(saida):
    custo = re.search(
        r"^Melhor custo:\s*(\d+)",
        saida,
        re.MULTILINE
    )

    caminhos = re.search(
        r"^Caminhos explorados:\s*(\d+)",
        saida,
        re.MULTILINE
    )

    chamadas = re.search(
        r"^Chamadas recursivas:\s*(\d+)",
        saida,
        re.MULTILINE
    )

    concluida = re.search(
        r"^Busca concluida:\s*(Sim|Nao)",
        saida,
        re.MULTILINE
    )

    if caminhos is None or chamadas is None or concluida is None:
        raise ValueError("Saida da busca completa incompleta")

    return {
        "tempo_busca": extrair_tempo(saida),
        "custo_busca": int(custo.group(1)) if custo else None,
        "caminhos": int(caminhos.group(1)),
        "chamadas": int(chamadas.group(1)),
        "concluida": concluida.group(1) == "Sim"
    }


def extrair_dijkstra(saida, origem, destino):
    padrao = (
        rf"^\[{origem},{destino}\]"
        rf"\((INF|\d+)\)"
    )

    resultado = re.search(
        padrao,
        saida,
        re.MULTILINE
    )

    if resultado is None:
        raise ValueError("Custo de Dijkstra nao encontrado")

    custo = resultado.group(1)

    return {
        "tempo_dijkstra": extrair_tempo(saida),
        "custo_dijkstra": None if custo == "INF" else int(custo)
    }


def comparar_instancia(
    grafo,
    busca,
    dijkstra,
    origem,
    destino,
    limite
):
    saida_busca = executar(
        busca,
        [origem, destino, limite],
        grafo,
        max(30, limite * 10 + 10)
    )

    saida_dijkstra = executar(
        dijkstra,
        [origem],
        grafo,
        120
    )

    resultado_busca = extrair_busca(saida_busca)
    resultado_dijkstra = extrair_dijkstra(
        saida_dijkstra,
        origem,
        destino
    )

    return {
        **resultado_busca,
        **resultado_dijkstra
    }


def executar_comparacoes(args):
    registros = []

    for densidade in args.densidades:
        for vertices in args.tamanhos:
            grafo = (
                Path(args.grafos)
                / f"densidade_{densidade}"
                / f"grafo_{vertices}.gr"
            )

            if not grafo.exists():
                print(f"Arquivo nao encontrado: {grafo}")
                continue

            destino = vertices if args.destino_final else args.destino

            for repeticao in range(1, args.repeticoes + 1):
                print(
                    f"Densidade={densidade}% "
                    f"V={vertices} "
                    f"Repeticao={repeticao}"
                )

                try:
                    resultado = comparar_instancia(
                        grafo,
                        Path(args.busca).resolve(),
                        Path(args.dijkstra).resolve(),
                        args.origem,
                        destino,
                        args.limite
                    )

                    registros.append({
                        "densidade": densidade,
                        "vertices": vertices,
                        "origem": args.origem,
                        "destino": destino,
                        "repeticao": repeticao,
                        **resultado
                    })

                except (
                    RuntimeError,
                    ValueError,
                    subprocess.TimeoutExpired,
                    OSError
                ) as erro:
                    print(f"Erro: {erro}")

    return pd.DataFrame(registros)


def gerar_grafico_tempos(df, saida):
    concluidas = df[df["concluida"]].copy()

    if concluidas.empty:
        print("Nenhuma busca completa foi concluida")
        return

    resumo = (
        concluidas.groupby(
            ["densidade", "vertices"],
            as_index=False
        )
        .agg(
            tempo_busca=("tempo_busca", "mean"),
            tempo_dijkstra=("tempo_dijkstra", "mean")
        )
    )

    for densidade, grupo in resumo.groupby("densidade"):
        grupo = grupo.sort_values("vertices")

        plt.figure(figsize=(9, 5))

        plt.plot(
            grupo["vertices"],
            grupo["tempo_busca"],
            marker="o",
            label="Busca completa"
        )

        plt.plot(
            grupo["vertices"],
            grupo["tempo_dijkstra"],
            marker="s",
            label="Dijkstra"
        )

        plt.xlabel("Numero de vertices")
        plt.ylabel("Tempo medio de CPU (s)")
        plt.title(f"Comparacao - densidade {densidade}%")
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.tight_layout()

        plt.savefig(
            saida / f"comparacao_tempo_{densidade}.png",
            dpi=200
        )

        plt.close()


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--grafos",
        default="tests/instancias_sinteticas"
    )

    parser.add_argument(
        "--busca",
        default="./tarefa05.exe"
    )

    parser.add_argument(
        "--dijkstra",
        default="../tarefa04/tarefa04.exe"
    )

    parser.add_argument(
        "--saida",
        default="resultados/comparacao_final"
    )

    parser.add_argument(
        "--densidades",
        type=int,
        nargs="+",
        default=[20, 50, 80]
    )

    parser.add_argument(
        "--tamanhos",
        type=int,
        nargs="+",
        default=[5, 10, 15, 20, 25]
    )

    parser.add_argument("--origem", type=int, default=1)
    parser.add_argument("--destino", type=int, default=5)
    parser.add_argument("--destino-final", action="store_true")
    parser.add_argument("--limite", type=float, default=10)
    parser.add_argument("--repeticoes", type=int, default=5)

    args = parser.parse_args()

    saida = Path(args.saida)
    saida.mkdir(parents=True, exist_ok=True)

    df = executar_comparacoes(args)

    if df.empty:
        raise RuntimeError("Nenhum experimento realizado")

    df["custos_coincidem"] = (
        df["custo_busca"] == df["custo_dijkstra"]
    )

    df["resultado_validado"] = (
        df["concluida"] & df["custos_coincidem"]
    )

    df.to_csv(
        saida / "comparacao_completa.csv",
        index=False
    )

    gerar_grafico_tempos(df, saida)

    print("\nResumo:")
    print(
        df.groupby(["densidade", "vertices"])
        .agg(
            tempo_busca=("tempo_busca", "mean"),
            tempo_dijkstra=("tempo_dijkstra", "mean"),
            taxa_conclusao=("concluida", "mean"),
            taxa_validacao=("resultado_validado", "mean")
        )
        .to_string()
    )


if __name__ == "__main__":
    main()
