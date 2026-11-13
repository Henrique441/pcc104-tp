
import csv
import re
import statistics
import subprocess
from collections import defaultdict
from pathlib import Path


PASTA_INSTANCIAS = Path("tests/instancias_sinteticas")
ARQUIVO_SA = Path("resultados/execucoes_sa.csv")
EXECUTAVEL_DIJKSTRA = Path("../tarefa04/tarefa04.exe").resolve()
ARQUIVO_SAIDA = Path("resultados/desempenho_final.csv")

REPETICOES = 30


def carregar_sa():
    dados = defaultdict(list)

    with ARQUIVO_SA.open(
        "r", newline="", encoding="utf-8"
    ) as arquivo:

        leitor = csv.DictReader(arquivo)

        for linha in leitor:
            dados[linha["instancia"]].append({
                "custo": (
                    int(linha["custo_sa"])
                    if linha["custo_sa"] else None
                ),
                "tempo": float(linha["tempo_sa"]),
                "encontrou": (
                    linha["encontrou"].lower() == "true"
                )
            })

    return dados


def executar_dijkstra(arquivo_grafo, origem, destino):
    with arquivo_grafo.open("r", encoding="utf-8") as entrada:
        processo = subprocess.run(
            [
                str(EXECUTAVEL_DIJKSTRA),
                str(origem),
                str(destino)
            ],
            stdin=entrada,
            capture_output=True,
            text=True,
            timeout=30
        )

    if processo.returncode != 0:
        raise RuntimeError(processo.stderr)

    saida = processo.stdout + "\n" + processo.stderr

    match_custo = re.search(
        rf"\[\s*{origem}\s*,\s*{destino}\s*\]"
        rf"\s*\(\s*(-?\d+)\s*\)",
        saida
    )

    match_tempo = re.search(
        r"Tempo de execucao:\s*([\d.]+)",
        saida
    )

    if not match_custo or not match_tempo:
        raise ValueError(
            "Nao foi possivel interpretar Dijkstra:\n" + saida
        )

    return (
        int(match_custo.group(1)),
        float(match_tempo.group(1))
    )


def media(valores):
    return statistics.mean(valores)


def desvio(valores):
    return statistics.stdev(valores) if len(valores) > 1 else 0.0


def main():
    dados_sa = carregar_sa()
    resultados = []

    for nome_instancia, execucoes in sorted(dados_sa.items()):

        arquivo = PASTA_INSTANCIAS / nome_instancia

        vertices = int(
            re.search(r"grafo_(\d+)", nome_instancia).group(1)
        )

        origem = 1
        destino = vertices

        tempos_dijkstra = []
        custo_otimo = None

        for _ in range(REPETICOES):
            custo, tempo = executar_dijkstra(
                arquivo, origem, destino
            )

            if custo_otimo is not None and custo != custo_otimo:
                raise RuntimeError(
                    f"Custo inconsistente em {nome_instancia}"
                )

            custo_otimo = custo
            tempos_dijkstra.append(tempo)

        tempos_sa = [e["tempo"] for e in execucoes]

        custos_sa = [
            e["custo"]
            for e in execucoes
            if e["encontrou"] and e["custo"] is not None
        ]

        otimas = sum(
            custo == custo_otimo for custo in custos_sa
        )

        taxa_otimalidade = (
            100.0 * otimas / len(execucoes)
        )

        tempo_medio_sa = media(tempos_sa)
        tempo_medio_dijkstra = media(tempos_dijkstra)

        razao_tempo = (
            tempo_medio_sa / tempo_medio_dijkstra
            if tempo_medio_dijkstra > 0 else None
        )

        resultados.append({
            "instancia": nome_instancia,
            "vertices": vertices,
            "custo_otimo": custo_otimo,
            "custo_medio_sa": (
                media(custos_sa) if custos_sa else None
            ),
            "tempo_medio_sa": tempo_medio_sa,
            "desvio_tempo_sa": desvio(tempos_sa),
            "tempo_medio_dijkstra": tempo_medio_dijkstra,
            "desvio_tempo_dijkstra": desvio(tempos_dijkstra),
            "razao_tempo_sa_dijkstra": razao_tempo,
            "taxa_otimalidade_sa": taxa_otimalidade,
            "falhas_sa": len(execucoes) - len(custos_sa)
        })

        print(
            f"{nome_instancia} | "
            f"SA: {tempo_medio_sa:.6f}s | "
            f"Dijkstra: {tempo_medio_dijkstra:.6f}s | "
            f"Otimalidade SA: {taxa_otimalidade:.2f}%"
        )

    ARQUIVO_SAIDA.parent.mkdir(
        parents=True, exist_ok=True
    )

    with ARQUIVO_SAIDA.open(
        "w", newline="", encoding="utf-8"
    ) as arquivo:

        escritor = csv.DictWriter(
            arquivo,
            fieldnames=list(resultados[0].keys())
        )

        escritor.writeheader()
        escritor.writerows(resultados)

    print("\nArquivo gerado:", ARQUIVO_SAIDA)


if __name__ == "__main__":
    main()
