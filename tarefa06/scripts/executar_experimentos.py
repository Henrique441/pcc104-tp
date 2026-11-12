
import argparse
import csv
import re
import subprocess
from pathlib import Path
from statistics import mean, stdev

MODOS = {
    0: "Busca Completa",
    1: "Poda por Custo",
    2: "Poda por Limite Inferior"
}

CAMPOS = [
    "instancia",
    "vertices",
    "arestas",
    "densidade",
    "modo",
    "algoritmo",
    "repeticao",
    "tempo",
    "caminhos",
    "chamadas",
    "podas",
    "podas_custo",
    "podas_limite",
    "melhor_custo",
    "concluida"
]


def extrair_numero(texto, campo, padrao=None):
    regex = rf"^{re.escape(campo)}:\s*(\d+)"
    resultado = re.search(regex, texto, re.MULTILINE)

    if resultado:
        return int(resultado.group(1))

    return padrao


def ler_cabecalho(caminho):
    with open(caminho, encoding="utf-8") as arquivo:
        for linha in arquivo:
            if linha.startswith("p sp "):
                _, _, v, e = linha.split()
                return int(v), int(e)

    raise ValueError(f"Cabecalho invalido: {caminho}")


def executar(executavel, instancia, modo, limite):
    vertices, arestas = ler_cabecalho(instancia)

    # Nos grafos sinteticos, a origem e 1
    # e o destino e o ultimo vertice.
    origem = 1
    destino = vertices

    comando = [
        str(executavel.resolve()),
        str(origem),
        str(destino),
        str(limite),
        str(modo)
    ]

    with open(instancia, "r", encoding="utf-8") as entrada:
        processo = subprocess.run(
            comando,
            stdin=entrada,
            capture_output=True,
            text=True,
            timeout=max(30, limite + 30)
        )

    if processo.returncode != 0:
        raise RuntimeError(
            f"Erro em {instancia}, modo {modo}:\n"
            f"{processo.stderr}"
        )

    saida = processo.stdout

    tempo_match = re.search(
        r"Tempo de execucao:\s*([\d.]+)",
        saida
    )

    conclusao = re.search(
        r"Busca concluida:\s*(Sim|Nao)",
        saida
    )

    if tempo_match is None or conclusao is None:
        raise RuntimeError(
            f"Saida inesperada:\n{saida}"
        )

    campos_obrigatorios = [
        "Caminhos explorados",
        "Chamadas recursivas",
        "Podas realizadas"
    ]

    for campo in campos_obrigatorios:
        if extrair_numero(saida, campo) is None:
            raise RuntimeError(
                f"Campo ausente: {campo}\n{saida}"
            )

    densidade = arestas / (vertices * (vertices - 1))

    return {
        "instancia": str(instancia),
        "vertices": vertices,
        "arestas": arestas,
        "densidade": round(densidade, 4),
        "modo": modo,
        "algoritmo": MODOS[modo],
        "tempo": float(tempo_match.group(1)),
        "caminhos": extrair_numero(
            saida, "Caminhos explorados"
        ),
        "chamadas": extrair_numero(
            saida, "Chamadas recursivas"
        ),
        "podas": extrair_numero(
            saida, "Podas realizadas"
        ),
        "podas_custo": extrair_numero(
            saida, "Podas por custo", 0
        ),
        "podas_limite": extrair_numero(
            saida, "Podas por limite inferior", 0
        ),
        "melhor_custo": extrair_numero(
            saida, "Melhor custo"
        ),
        "concluida": (
            conclusao.group(1) == "Sim"
        )
    }


def salvar_csv(caminho, dados, campos):
    caminho.parent.mkdir(
        parents=True, exist_ok=True
    )

    with open(
        caminho, "w", newline="", encoding="utf-8"
    ) as arquivo:
        escritor = csv.DictWriter(
            arquivo, fieldnames=campos
        )
        escritor.writeheader()
        escritor.writerows(dados)


def criar_resumo(resultados):
    grupos = {}

    for resultado in resultados:
        chave = (
            resultado["instancia"],
            resultado["modo"]
        )
        grupos.setdefault(chave, []).append(resultado)

    resumo = []

    for (instancia, modo), grupo in sorted(grupos.items()):
        tempos = [r["tempo"] for r in grupo]

        custos = [
            r["melhor_custo"]
            for r in grupo
            if r["melhor_custo"] is not None
        ]

        resumo.append({
            "instancia": instancia,
            "vertices": grupo[0]["vertices"],
            "arestas": grupo[0]["arestas"],
            "densidade": grupo[0]["densidade"],
            "modo": modo,
            "algoritmo": MODOS[modo],
            "tempo_medio": mean(tempos),
            "tempo_desvio": (
                stdev(tempos) if len(tempos) > 1 else 0
            ),
            "caminhos_medios": mean(
                r["caminhos"] for r in grupo
            ),
            "chamadas_medias": mean(
                r["chamadas"] for r in grupo
            ),
            "podas_medias": mean(
                r["podas"] for r in grupo
            ),
            "podas_custo_medias": mean(
                r["podas_custo"] for r in grupo
            ),
            "podas_limite_medias": mean(
                r["podas_limite"] for r in grupo
            ),
            "taxa_conclusao": mean(
                int(r["concluida"]) for r in grupo
            ),
            "custos_observados": (
                ";".join(map(str, sorted(set(custos))))
                if custos else ""
            )
        })

    return resumo


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--pasta",
        default="tests/instancias_sinteticas"
    )
    parser.add_argument(
        "--executavel",
        default="./tarefa06.exe"
    )
    parser.add_argument(
        "--saida",
        default="resultados"
    )
    parser.add_argument(
        "--repeticoes",
        type=int,
        default=5
    )
    parser.add_argument(
        "--limite",
        type=float,
        default=10
    )

    args = parser.parse_args()

    if args.repeticoes < 1 or args.limite <= 0:
        parser.error(
            "Repeticoes deve ser >= 1 e limite > 0"
        )

    instancias = sorted(
        Path(args.pasta).rglob("*.gr")
    )

    if not instancias:
        raise RuntimeError(
            "Nenhuma instancia .gr encontrada."
        )

    resultados = []

    for instancia in instancias:
        for repeticao in range(1, args.repeticoes + 1):
            for modo in MODOS:
                print(
                    f"{instancia.name} | "
                    f"modo={modo} | "
                    f"repeticao={repeticao}",
                    flush=True
                )

                try:
                    resultado = executar(
                        Path(args.executavel),
                        instancia,
                        modo,
                        args.limite
                    )
                except subprocess.TimeoutExpired:
                    print(
                        "Tempo externo excedido. "
                        "Execucao nao registrada."
                    )
                    continue

                resultado["repeticao"] = repeticao
                resultados.append(resultado)

    pasta_saida = Path(args.saida)

    salvar_csv(
        pasta_saida / "resultados_completos.csv",
        resultados,
        CAMPOS
    )

    resumo = criar_resumo(resultados)

    if resumo:
        salvar_csv(
            pasta_saida / "resumo_experimentos.csv",
            resumo,
            list(resumo[0].keys())
        )

    print("\nExperimentos finalizados.")
    print(f"Execucoes registradas: {len(resultados)}")
    print(f"Resultados salvos em: {pasta_saida}")


if __name__ == "__main__":
    main()
