
import argparse
import random
from pathlib import Path


def gerar_grafo(num_vertices, densidade, semente):
    if num_vertices < 2:
        raise ValueError("O grafo precisa ter pelo menos 2 vertices")

    if not 0 <= densidade <= 1:
        raise ValueError("A densidade deve estar entre 0 e 1")

    randomizador = random.Random(semente)

    max_arestas = num_vertices * (num_vertices - 1)
    alvo = round(densidade * max_arestas)

    alvo = max(alvo, num_vertices - 1)

    arestas = {}

    for u in range(1, num_vertices):
        v = u + 1
        peso = randomizador.randint(1, 20)
        arestas[(u, v)] = peso

    candidatos = [
        (u, v)
        for u in range(1, num_vertices + 1)
        for v in range(1, num_vertices + 1)
        if u != v and (u, v) not in arestas
    ]

    randomizador.shuffle(candidatos)

    for u, v in candidatos[:alvo - len(arestas)]:
        arestas[(u, v)] = randomizador.randint(1, 20)

    return sorted(
        (u, v, peso)
        for (u, v), peso in arestas.items()
    )


def salvar_dimacs(caminho, num_vertices, arestas):
    with open(caminho, "w", encoding="utf-8") as arquivo:
        arquivo.write(
            f"p sp {num_vertices} {len(arestas)}\n"
        )

        for u, v, peso in arestas:
            arquivo.write(f"a {u} {v} {peso}\n")


def gerar_instancias(pasta, tamanhos, densidades, semente):
    pasta.mkdir(parents=True, exist_ok=True)

    for densidade in densidades:
        percentual = round(densidade * 100)

        for tamanho in tamanhos:
            arestas = gerar_grafo(
                tamanho,
                densidade,
                semente + tamanho + percentual
            )

            nome = f"grafo_{tamanho}.gr"
            destino = pasta / f"densidade_{percentual}" / nome

            destino.parent.mkdir(
                parents=True,
                exist_ok=True
            )

            salvar_dimacs(destino, tamanho, arestas)

            densidade_real = (
                len(arestas) / (tamanho * (tamanho - 1))
            )

            print(
                f"{destino} | "
                f"V={tamanho} | "
                f"E={len(arestas)} | "
                f"d={densidade_real:.2%}"
            )


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--saida",
        default="tests/instancias_sinteticas"
    )

    parser.add_argument(
        "--tamanhos",
        type=int,
        nargs="+",
        default=[5, 10, 15, 20, 25]
    )

    parser.add_argument(
        "--densidades",
        type=float,
        nargs="+",
        default=[0.2, 0.5, 0.8]
    )

    parser.add_argument(
        "--semente",
        type=int,
        default=42
    )

    args = parser.parse_args()

    gerar_instancias(
        Path(args.saida),
        args.tamanhos,
        args.densidades,
        args.semente
    )
