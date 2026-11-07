
import re
import sys

def ler_resultados(caminho):
    resultados = {}

    with open(caminho, "r", encoding="utf-8") as arquivo:
        for numero, linha in enumerate(arquivo, 1):
            linha = linha.strip()

            if not linha:
                continue

            padrao = r"\[(\d+),(\d+)\]\((\d+|INF)\)(?:\s+(.*))?"
            match = re.fullmatch(padrao, linha)

            if match is None:
                raise ValueError(f"Linha invalida em {caminho}:{numero}")

            origem = int(match.group(1))
            destino = int(match.group(2))
            distancia = match.group(3)
            caminho_vertices = match.group(4)

            if destino in resultados:
                raise ValueError(f"Destino duplicado: {destino}")

            distancia = None if distancia == "INF" else int(distancia)
            vertices = (
                list(map(int, caminho_vertices.split()))
                if caminho_vertices else []
            )

            resultados[destino] = (origem, distancia, vertices)

    return resultados


def ler_arestas(caminho):
    arestas = {}

    with open(caminho, "r", encoding="utf-8") as arquivo:
        for linha in arquivo:
            partes = linha.split()

            if partes and partes[0] == "a":
                origem, destino, peso = map(int, partes[1:4])
                chave = (origem, destino)

                if chave not in arestas:
                    arestas[chave] = set()

                arestas[chave].add(peso)

    return arestas


def validar_caminho(origem, destino, distancia, vertices, arestas):
    if distancia is None:
        return not vertices

    if not vertices or vertices[0] != origem or vertices[-1] != destino:
        return False

    custos = {0}

    for u, v in zip(vertices, vertices[1:]):
        pesos = arestas.get((u, v))

        if pesos is None:
            return False

        custos = {
            custo + peso
            for custo in custos
            for peso in pesos
            if custo + peso <= distancia
        }

        if not custos:
            return False

    return distancia in custos


def comparar(referencia, obtido, arestas):
    destinos = sorted(set(referencia) | set(obtido))

    distancias_corretas = 0
    caminhos_iguais = 0
    caminhos_validos = 0
    divergencias = []

    for destino in destinos:
        if destino not in referencia or destino not in obtido:
            divergencias.append((destino, "Destino ausente"))
            continue

        origem_ref, distancia_ref, caminho_ref = referencia[destino]
        origem_obt, distancia_obt, caminho_obt = obtido[destino]

        if origem_ref != origem_obt or distancia_ref != distancia_obt:
            divergencias.append((destino, "Distancia diferente"))
            continue

        distancias_corretas += 1

        if caminho_ref == caminho_obt:
            caminhos_iguais += 1

        if validar_caminho(
            origem_obt, destino, distancia_obt, caminho_obt, arestas
        ):
            caminhos_validos += 1
        else:
            divergencias.append((destino, "Caminho invalido"))

    print(f"Destinos esperados: {len(referencia)}")
    print(f"Destinos obtidos: {len(obtido)}")
    print(f"Distancias corretas: {distancias_corretas}")
    print(f"Caminhos identicos: {caminhos_iguais}")
    print(f"Caminhos validos: {caminhos_validos}")
    print(f"Divergencias: {len(divergencias)}")

    for destino, motivo in divergencias[:20]:
        print(f"Destino {destino}: {motivo}")


if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Uso: python comparar_resultados.py referencia obtido grafo")
        sys.exit(1)

    referencia = ler_resultados(sys.argv[1])
    obtido = ler_resultados(sys.argv[2])
    arestas = ler_arestas(sys.argv[3])

    comparar(referencia, obtido, arestas)
