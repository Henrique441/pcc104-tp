
import argparse
from collections import deque
from pathlib import Path


def ler_dimacs(caminho):
    num_vertices = 0
    arestas = []

    with open(caminho, "r", encoding="utf-8") as arquivo:
        for linha in arquivo:
            partes = linha.split()

            if not partes or partes[0] == "c":
                continue

            if partes[0] == "p":
                num_vertices = int(partes[2])

            elif partes[0] == "a":
                origem = int(partes[1])
                destino = int(partes[2])
                peso = int(partes[3])
                arestas.append((origem, destino, peso))

    if num_vertices <= 0:
        raise ValueError("Cabecalho DIMACS invalido")

    return num_vertices, arestas


def selecionar_vertices(num_vertices, arestas, origem, quantidade):
    adjacencias = [[] for _ in range(num_vertices + 1)]

    for u, v, _ in arestas:
        adjacencias[u].append(v)
        adjacencias[v].append(u)

    visitados = {origem}
    fila = deque([origem])
    selecionados = []

    while fila and len(selecionados) < quantidade:
        atual = fila.popleft()
        selecionados.append(atual)

        for vizinho in adjacencias[atual]:
            if vizinho not in visitados:
                visitados.add(vizinho)
                fila.append(vizinho)

    if len(selecionados) < quantidade:
        raise ValueError(
            f"Apenas {len(selecionados)} vertices encontrados"
        )

    return selecionados


def salvar_subgrafo(caminho, selecionados, arestas):
    mapeamento = {
        vertice: indice
        for indice, vertice in enumerate(selecionados, start=1)
    }

    novas_arestas = []

    for u, v, peso in arestas:
        if u in mapeamento and v in mapeamento:
            novas_arestas.append(
                (mapeamento[u], mapeamento[v], peso)
            )

    with open(caminho, "w", encoding="utf-8") as arquivo:
        arquivo.write(
            f"p sp {len(selecionados)} {len(novas_arestas)}\n"
        )

        for u, v, peso in novas_arestas:
            arquivo.write(f"a {u} {v} {peso}\n")

    return len(novas_arestas)


def gerar_instancias(entrada, saida, origem, tamanhos):
    num_vertices, arestas = ler_dimacs(entrada)

    if not 1 <= origem <= num_vertices:
        raise ValueError("Vertice de origem invalido")

    saida = Path(saida)
    saida.mkdir(parents=True, exist_ok=True)

    for tamanho in tamanhos:
        selecionados = selecionar_vertices(
            num_vertices,
            arestas,
            origem,
            tamanho
        )

        arquivo = saida / f"grafo_{tamanho}.gr"

        num_arestas = salvar_subgrafo(
            arquivo,
            selecionados,
            arestas
        )

        print(
            f"{arquivo}: "
            f"{tamanho} vertices, {num_arestas} arestas"
        )


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument("entrada")
    parser.add_argument("saida")
    parser.add_argument("--origem", type=int, default=1)
    parser.add_argument(
        "--tamanhos",
        type=int,
        nargs="+",
        default=[5, 10, 15, 20, 25, 30, 35, 40]
    )

    args = parser.parse_args()

    gerar_instancias(
        args.entrada,
        args.saida,
        args.origem,
        args.tamanhos
    )
