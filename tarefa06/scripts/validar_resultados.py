
import csv
import heapq
from pathlib import Path

ARQUIVO_RESULTADOS = Path(
    "resultados/resultados_completos.csv"
)

ARQUIVO_SAIDA = Path(
    "resultados/validacao_dijkstra.csv"
)


def ler_grafo(caminho):
    n = 0
    adj = None

    with open(caminho, encoding="utf-8") as arquivo:
        for linha in arquivo:
            partes = linha.split()

            if not partes or partes[0] == "c":
                continue

            if partes[0] == "p":
                n = int(partes[2])
                adj = [[] for _ in range(n + 1)]

            elif partes[0] == "a":
                u, v, peso = map(int, partes[1:4])
                adj[u].append((v, peso))

    return n, adj


def dijkstra(adj, origem, destino):
    dist = [float("inf")] * len(adj)
    dist[origem] = 0

    fila = [(0, origem)]

    while fila:
        custo, atual = heapq.heappop(fila)

        if custo != dist[atual]:
            continue

        if atual == destino:
            return custo

        for vizinho, peso in adj[atual]:
            novo = custo + peso

            if novo < dist[vizinho]:
                dist[vizinho] = novo
                heapq.heappush(
                    fila, (novo, vizinho)
                )

    return None


def main():
    validacoes = []
    divergencias = 0

    with open(
        ARQUIVO_RESULTADOS,
        newline="",
        encoding="utf-8"
    ) as arquivo:

        resultados = list(csv.DictReader(arquivo))

    custos_otimos = {}

    for resultado in resultados:
        instancia = resultado["instancia"]

        if instancia not in custos_otimos:
            n, adj = ler_grafo(instancia)
            custos_otimos[instancia] = dijkstra(
                adj, 1, n
            )

        custo_otimo = custos_otimos[instancia]

        texto_custo = resultado["melhor_custo"]
        custo_encontrado = (
            int(texto_custo)
            if texto_custo else None
        )

        correto = custo_encontrado == custo_otimo

        if not correto:
            divergencias += 1

        validacoes.append({
            "instancia": instancia,
            "modo": resultado["modo"],
            "repeticao": resultado["repeticao"],
            "concluida": resultado["concluida"],
            "custo_encontrado": custo_encontrado,
            "custo_otimo": custo_otimo,
            "custo_correto": correto
        })

    ARQUIVO_SAIDA.parent.mkdir(
        parents=True, exist_ok=True
    )

    with open(
        ARQUIVO_SAIDA,
        "w",
        newline="",
        encoding="utf-8"
    ) as arquivo:
        escritor = csv.DictWriter(
            arquivo,
            fieldnames=validacoes[0].keys()
        )
        escritor.writeheader()
        escritor.writerows(validacoes)

    print(f"Execucoes verificadas: {len(validacoes)}")
    print(f"Custos divergentes: {divergencias}")
    print(f"Arquivo salvo: {ARQUIVO_SAIDA}")

    if divergencias:
        print("\nDivergencias encontradas:")

        for item in validacoes:
            if not item["custo_correto"]:
                print(
                    f"{item['instancia']} | "
                    f"modo={item['modo']} | "
                    f"concluida={item['concluida']} | "
                    f"encontrado={item['custo_encontrado']} | "
                    f"otimo={item['custo_otimo']}"
                )


if __name__ == "__main__":
    main()
