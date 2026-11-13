
import argparse
import csv
import math
import re
import statistics
import subprocess
from pathlib import Path


def ler_num_vertices(arquivo):
    with arquivo.open("r", encoding="utf-8") as f:
        for linha in f:
            partes = linha.split()
            if len(partes) >= 4 and partes[:2] == ["p", "sp"]:
                return int(partes[2])

    raise ValueError(f"Cabecalho DIMACS ausente: {arquivo}")


def executar(executavel, argumentos, arquivo, timeout):
    with arquivo.open("r", encoding="utf-8") as entrada:
        processo = subprocess.run(
            [str(executavel), *map(str, argumentos)],
            stdin=entrada,
            capture_output=True,
            text=True,
            timeout=timeout,
            check=False
        )

    if processo.returncode != 0:
        raise RuntimeError(
            f"Falha em {executavel}:\n"
            f"{processo.stderr}\n{processo.stdout}"
        )

    return processo.stdout + "\n" + processo.stderr


def extrair_dijkstra(saida, origem, destino):
    padrao = (
        rf"\[\s*{origem}\s*,\s*{destino}\s*\]"
        rf"\s*\(\s*(-?\d+)\s*\)"
    )

    match = re.search(padrao, saida)

    if not match:
        raise ValueError(
            "Nao foi possivel extrair o custo de Dijkstra:\n"
            + saida
        )

    return int(match.group(1))


def extrair_sa(saida):
    encontrou = re.search(
        r"Destino encontrado:\s*(Sim|Nao)",
        saida,
        re.IGNORECASE
    )

    tempo = re.search(
        r"Tempo de execucao:\s*([\d.]+)",
        saida
    )

    if not encontrou or not tempo:
        raise ValueError("Saida inesperada do SA:\n" + saida)

    sucesso = encontrou.group(1).lower() == "sim"

    custo = None
    if sucesso:
        match = re.search(r"^Custo:\s*(\d+)", saida, re.MULTILINE)

        if not match:
            raise ValueError("Custo ausente:\n" + saida)

        custo = int(match.group(1))

    return {
        "encontrou": sucesso,
        "custo": custo,
        "tempo": float(tempo.group(1))
    }


def salvar_csv(caminho, linhas, colunas):
    caminho.parent.mkdir(parents=True, exist_ok=True)

    with caminho.open("w", newline="", encoding="utf-8") as f:
        escritor = csv.DictWriter(f, fieldnames=colunas)
        escritor.writeheader()
        escritor.writerows(linhas)


def calcular_estatisticas(custos, tempos, otimo, repeticoes):
    sucessos = len(custos)
    falhas = repeticoes - sucessos

    if not custos:
        return {
            "melhor_custo": None,
            "pior_custo": None,
            "custo_medio": None,
            "desvio_padrao": None,
            "erro_medio_percentual": None,
            "erro_melhor_percentual": None,
            "execucoes_otimas": 0,
            "taxa_otimalidade": 0.0,
            "sucessos": 0,
            "falhas": falhas,
            "tempo_medio": statistics.mean(tempos)
        }

    otimas = sum(c == otimo for c in custos)

    custo_medio = statistics.mean(custos)
    desvio = statistics.stdev(custos) if sucessos > 1 else 0.0

    if otimo > 0:
        erro_medio = 100.0 * (custo_medio - otimo) / otimo
        erro_melhor = 100.0 * (min(custos) - otimo) / otimo
    else:
        erro_medio = 0.0 if all(c == 0 for c in custos) else None
        erro_melhor = 0.0 if min(custos) == 0 else None

    return {
        "melhor_custo": min(custos),
        "pior_custo": max(custos),
        "custo_medio": custo_medio,
        "desvio_padrao": desvio,
        "erro_medio_percentual": erro_medio,
        "erro_melhor_percentual": erro_melhor,
        "execucoes_otimas": otimas,
        "taxa_otimalidade": 100.0 * otimas / repeticoes,
        "sucessos": sucessos,
        "falhas": falhas,
        "tempo_medio": statistics.mean(tempos)
    }


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--pasta",
        default="tests/instancias_sinteticas"
    )

    parser.add_argument(
        "--sa",
        default="./tarefa08.exe"
    )

    parser.add_argument(
        "--dijkstra",
        default="../tarefa04/tarefa04.exe"
    )

    parser.add_argument(
        "--saida",
        default="resultados/comparacao_final.csv"
    )

    parser.add_argument("--repeticoes", type=int, default=30)
    parser.add_argument("--limite", type=float, default=5.0)
    parser.add_argument("--semente-base", type=int, default=12345)

    args = parser.parse_args()

    if args.repeticoes < 1 or args.limite <= 0:
        parser.error("Repeticoes e limite devem ser positivos.")

    pasta = Path(args.pasta).resolve()
    sa = Path(args.sa).resolve()
    dijkstra = Path(args.dijkstra).resolve()
    saida = Path(args.saida).resolve()

    arquivos = sorted(
        pasta.rglob("*.gr"),
        key=lambda p: (
            int(re.search(r"densidade_(\d+)", str(p)).group(1)),
            ler_num_vertices(p)
        )
    )

    if not arquivos:
        raise RuntimeError("Nenhuma instancia .gr encontrada.")

    individuais = []
    resumo = []

    total = len(arquivos)

    for indice, arquivo in enumerate(arquivos, start=1):
        vertices = ler_num_vertices(arquivo)
        origem = 1
        destino = vertices

        nome = str(arquivo.relative_to(pasta))

        saida_dijkstra = executar(
            dijkstra,
            [origem, destino],
            arquivo,
            timeout=max(30.0, args.limite + 5.0)
        )

        otimo = extrair_dijkstra(
            saida_dijkstra, origem, destino
        )

        custos = []
        tempos = []

        for repeticao in range(1, args.repeticoes + 1):
            semente = (
                args.semente_base
                + (indice - 1) * args.repeticoes
                + repeticao
            )

            try:
                saida_sa = executar(
                    sa,
                    [origem, destino, args.limite, semente],
                    arquivo,
                    timeout=args.limite + 10.0
                )

                dados = extrair_sa(saida_sa)

            except subprocess.TimeoutExpired:
                dados = {
                    "encontrou": False,
                    "custo": None,
                    "tempo": args.limite + 10.0
                }
                print(
                    f"Aviso: timeout externo em {nome}, "
                    f"repeticao {repeticao}"
                )

            tempos.append(dados["tempo"])

            if dados["encontrou"]:
                custos.append(dados["custo"])

                if dados["custo"] < otimo:
                    raise RuntimeError(
                        f"SA encontrou custo {dados['custo']} "
                        f"menor que Dijkstra ({otimo}) em {nome}. "
                        "Verifique a implementacao."
                    )

            if dados["custo"] is not None and otimo > 0:
                erro = (
                    100.0 * (dados["custo"] - otimo) / otimo
                )
            elif dados["custo"] == 0 and otimo == 0:
                erro = 0.0
            else:
                erro = None

            individuais.append({
                "instancia": nome,
                "vertices": vertices,
                "repeticao": repeticao,
                "semente": semente,
                "custo_sa": dados["custo"],
                "custo_otimo": otimo,
                "erro_percentual": erro,
                "encontrou": dados["encontrou"],
                "tempo_sa": dados["tempo"]
            })

        estatisticas = calcular_estatisticas(
            custos, tempos, otimo, args.repeticoes
        )

        resumo.append({
            "instancia": nome,
            "vertices": vertices,
            "custo_otimo": otimo,
            "repeticoes": args.repeticoes,
            **estatisticas
        })

        print(
            f"[{indice}/{total}] {nome} | "
            f"Otimo: {otimo} | "
            f"Melhor SA: {estatisticas['melhor_custo']} | "
            f"Media: {estatisticas['custo_medio']} | "
            f"Taxa otima: "
            f"{estatisticas['taxa_otimalidade']:.1f}% | "
            f"Falhas: {estatisticas['falhas']}"
        )

    campos_individuais = [
        "instancia", "vertices", "repeticao", "semente",
        "custo_sa", "custo_otimo", "erro_percentual",
        "encontrou", "tempo_sa"
    ]

    campos_resumo = [
        "instancia", "vertices", "custo_otimo",
        "repeticoes", "melhor_custo", "pior_custo",
        "custo_medio", "desvio_padrao",
        "erro_medio_percentual", "erro_melhor_percentual",
        "execucoes_otimas", "taxa_otimalidade",
        "sucessos", "falhas", "tempo_medio"
    ]

    salvar_csv(
        saida.parent / "execucoes_sa.csv",
        individuais,
        campos_individuais
    )

    salvar_csv(saida, resumo, campos_resumo)

    print("\nExperimentos finalizados.")
    print(f"Execucoes individuais: {saida.parent / 'execucoes_sa.csv'}")
    print(f"Resumo estatistico: {saida}")


if __name__ == "__main__":
    main()
