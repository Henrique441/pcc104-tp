
import argparse
import random
from pathlib import Path


def gerar_grafo(n, densidade, caminho, semente):
    """
    Gera um grafo direcionado, sem lacos e
    com pesos positivos no formato DIMACS.
    """
    rng = random.Random(semente)

    max_arestas = n * (n - 1)
    quantidade = round(densidade * max_arestas)

    # Seleciona arestas sem repeticao.
    indices = rng.sample(range(max_arestas), quantidade)

    caminho.parent.mkdir(parents=True, exist_ok=True)

    with caminho.open("w", encoding="utf-8") as arquivo:
        arquivo.write("c Grafo sintetico para Tarefa 09\n")
        arquivo.write(f"c Semente: {semente}\n")
        arquivo.write(f"c Densidade: {densidade:.2f}\n")
        arquivo.write(f"p sp {n} {quantidade}\n")

        for indice in indices:
            # Mapeia o indice para um par (origem, destino)
            # sem permitir origem == destino.
            origem = indice // (n - 1)
            destino = indice % (n - 1)

            if destino >= origem:
                destino += 1

            # Converte vertices para indices de 1 a n.
            origem += 1
            destino += 1

            peso = rng.randint(1, 100)

            arquivo.write(
                f"a {origem} {destino} {peso}\n"
            )

    print(
        f"Gerado: {caminho} | "
        f"V={n} | E={quantidade} | "
        f"densidade={densidade:.0%}"
    )


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--saida",
        type=Path,
        default=Path("tests/instancias_grandes")
    )

    parser.add_argument(
        "--semente",
        type=int,
        default=42
    )

    args = parser.parse_args()

    tamanhos = [50, 100, 200, 500]
    densidades = [0.20, 0.50, 0.80]

    for n in tamanhos:
        for densidade in densidades:
            percentual = round(densidade * 100)

            caminho = (
                args.saida
                / f"densidade_{percentual}"
                / f"grafo_{n}.gr"
            )

            gerar_grafo(
                n=n,
                densidade=densidade,
                caminho=caminho,
                semente=args.semente + n * 100 + percentual
            )


if __name__ == "__main__":
    main()
