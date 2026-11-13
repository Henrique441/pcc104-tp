
import argparse
import csv
import re
import subprocess
from pathlib import Path
import statistics
import time

def ler_vertices(caminho):
    with open(caminho, encoding="utf-8") as arquivo:
        for linha in arquivo:
            partes = linha.split()

            if len(partes) >= 4 and partes[:2] == ["p", "sp"]:
                return int(partes[2])

    raise ValueError(f"Cabecalho DIMACS invalido: {caminho}")


def executar(executavel, argumentos, instancia, limite):
    with open(instancia, encoding="utf-8") as entrada:
        processo = subprocess.run(
            [str(executavel.resolve()), *map(str, argumentos)],
            stdin=entrada,
            capture_output=True,
            text=True,
            timeout=limite + 10
        )

    if processo.returncode != 0:
        raise RuntimeError(
            f"Erro executando {executavel}:\n"
            f"{processo.stderr}"
        )

    return processo.stdout, processo.stderr


def extrair_tempo(texto):
    match = re.search(
        r"Tempo de execucao:\s*([\d.]+)",
        texto
    )
    if not match:
        raise ValueError(f"Tempo nao encontrado:\n{texto}")

    return float(match.group(1))


def extrair_guloso(saida):
    encontrou = re.search(
        r"Destino encontrado:\s*(Sim|Nao)",
        saida
    )
    custo = re.search(r"^Custo:\s*(\d+)", saida, re.MULTILINE)
    visitados = re.search(
        r"Vertices visitados:\s*(\d+)",
        saida
    )

    if not encontrou or not custo or not visitados:
        raise ValueError(f"Saida gulosa inesperada:\n{saida}")

    return {
        "encontrou": encontrou.group(1) == "Sim",
        "custo": int(custo.group(1)),
        "visitados": int(visitados.group(1)),
        "tempo": extrair_tempo(saida)
    }


def extrair_dijkstra(saida, erros, origem, destino):
    # Formato da Tarefa 4:
    # [1,5](10)1 3 2 4 5
    padrao = (
        rf"\[\s*{origem}\s*,\s*{destino}\s*\]"
        r"\s*\(\s*(\d+)\s*\)"
    )

    match = re.search(padrao, saida)

    if match:
        custo = int(match.group(1))
    else:
        # Se nao houver caminho, verificar a saida.
        # Nao assumir custo infinito sem confirmacao.
        raise ValueError(
            "Nao foi possivel extrair o custo de Dijkstra.\n"
            f"stdout:\n{saida}\nstderr:\n{erros}"
        )

    return {
        "custo": custo,
        "tempo": extrair_tempo(erros)
    }


def comparar(guloso, dijkstra):
    if not guloso["encontrou"]:
        return "Nao encontrou", None

    custo_guloso = guloso["custo"]
    custo_otimo = dijkstra["custo"]

    if custo_guloso == custo_otimo:
        return "Otima", 0.0

    if custo_otimo == 0:
        return "Subotima", None

    erro = (custo_guloso - custo_otimo) / custo_otimo
    return "Subotima", erro



def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--pasta",
        default="tests/instancias_sinteticas"
    )
    parser.add_argument(
        "--guloso",
        default="./tarefa07.exe"
    )
    parser.add_argument(
        "--dijkstra",
        default="../tarefa04/tarefa04.exe"
    )
    parser.add_argument(
        "--saida",
        default="resultados/comparacao_guloso.csv"
    )
    parser.add_argument(
        "--repeticoes",
        type=int,
        default=30
    )
    parser.add_argument(
        "--limite",
        type=float,
        default=60
    )

    args = parser.parse_args()

    if args.repeticoes < 1 or args.limite <= 0:
        parser.error("Repeticoes e limite devem ser positivos.")

    instancias = sorted(Path(args.pasta).rglob("*.gr"))

    if not instancias:
        raise RuntimeError("Nenhuma instancia encontrada.")

    resultados = []

    for instancia in instancias:
        n = ler_vertices(instancia)

        origem = 1
        destino = n

        tempos_guloso = []
        tempos_dijkstra = []

        print(f"\nTestando: {instancia}", flush=True)

        for repeticao in range(args.repeticoes):
            # Alterna a ordem dos algoritmos.
            # Ajuda a reduzir vieses de aquecimento/cache.
            if repeticao % 2 == 0:
                ordem = ["guloso", "dijkstra"]
            else:
                ordem = ["dijkstra", "guloso"]

            for algoritmo in ordem:
                if algoritmo == "guloso":
                    saida_g, erros_g = executar(
                        Path(args.guloso),
                        [origem, destino, args.limite],
                        instancia,
                        args.limite
                    )

                    guloso = extrair_guloso(saida_g)
                    tempos_guloso.append(guloso["tempo"])

                else:
                    saida_d, erros_d = executar(
                        Path(args.dijkstra),
                        [origem, destino],
                        instancia,
                        args.limite
                    )

                    dijkstra = extrair_dijkstra(
                        saida_d,
                        erros_d,
                        origem,
                        destino
                    )

                    tempos_dijkstra.append(dijkstra["tempo"])

        classificacao, erro = comparar(
            guloso,
            dijkstra
        )

        media_g = statistics.mean(tempos_guloso)
        media_d = statistics.mean(tempos_dijkstra)

        desvio_g = (
            statistics.stdev(tempos_guloso)
            if len(tempos_guloso) > 1 else 0.0
        )

        desvio_d = (
            statistics.stdev(tempos_dijkstra)
            if len(tempos_dijkstra) > 1 else 0.0
        )

        resultados.append({
            "instancia": str(instancia),
            "vertices": n,
            "densidade": instancia.parent.name,
            "custo_guloso": (
                guloso["custo"]
                if guloso["encontrou"] else ""
            ),
            "custo_otimo": dijkstra["custo"],
            "erro_relativo": (
                erro if erro is not None else ""
            ),
            "erro_percentual": (
                erro * 100 if erro is not None else ""
            ),
            "classificacao": classificacao,
            "vertices_visitados": guloso["visitados"],
            "tempo_medio_guloso": media_g,
            "desvio_guloso": desvio_g,
            "tempo_medio_dijkstra": media_d,
            "desvio_dijkstra": desvio_d,
            "repeticoes": args.repeticoes
        })

        print(
            f"  Guloso: {media_g:.8f} s "
            f"(desvio: {desvio_g:.8f})"
        )
        print(
            f"  Dijkstra: {media_d:.8f} s "
            f"(desvio: {desvio_d:.8f})"
        )
        print(f"  Classificacao: {classificacao}")

    destino_csv = Path(args.saida)

    destino_csv.parent.mkdir(
        parents=True,
        exist_ok=True
    )

    with open(
        destino_csv,
        "w",
        newline="",
        encoding="utf-8"
    ) as arquivo:
        escritor = csv.DictWriter(
            arquivo,
            fieldnames=resultados[0].keys()
        )
        escritor.writeheader()
        escritor.writerows(resultados)

    print("\nResumo final:")
    print(f"Total de instancias: {len(resultados)}")

    for categoria in ["Otima", "Subotima", "Nao encontrou"]:
        quantidade = sum(
            item["classificacao"] == categoria
            for item in resultados
        )
        print(f"{categoria}: {quantidade}")

    print(f"\nCSV salvo em: {destino_csv}")


if __name__ == "__main__":
    main()
