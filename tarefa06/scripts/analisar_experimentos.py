
from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


BASE = Path("resultados")
SAIDA = BASE / "analise_final"

DENSIDADES = {
    20: BASE / "sinteticos_20" / "resultados_completos.csv",
    50: BASE / "sinteticos_50" / "resultados_completos.csv",
    80: BASE / "sinteticos_80" / "resultados_completos.csv"
}


def carregar_resultados():
    dados = []

    for densidade, caminho in DENSIDADES.items():
        if not caminho.exists():
            print(f"Arquivo nao encontrado: {caminho}")
            continue

        df = pd.read_csv(caminho)
        df["densidade"] = densidade
        dados.append(df)

    if not dados:
        raise RuntimeError("Nenhum arquivo de resultados encontrado")

    return pd.concat(dados, ignore_index=True)


def calcular_resumo(df):
    df = df.copy()

    df["concluida"] = (
        df["concluida"]
        .astype(str)
        .str.lower()
        .eq("true")
    )

    resumo = (
        df.groupby(["densidade", "vertices", "arestas"])
        .agg(
            repeticoes=("tempo", "size"),
            concluidas=("concluida", "sum"),
            taxa_conclusao=("concluida", "mean")
        )
        .reset_index()
    )

    concluidas = df[df["concluida"]].copy()

    estatisticas = (
        concluidas.groupby(
            ["densidade", "vertices", "arestas"]
        )
        .agg(
            tempo_medio=("tempo", "mean"),
            tempo_desvio=("tempo", "std"),
            caminhos_medios=("caminhos", "mean"),
            chamadas_medias=("chamadas", "mean"),
            melhor_custo=("melhor_custo", "first")
        )
        .reset_index()
    )

    resumo = resumo.merge(
        estatisticas,
        on=["densidade", "vertices", "arestas"],
        how="left"
    )

    return resumo


def grafico_comparativo(resumo, coluna, titulo, ylabel, arquivo):
    plt.figure(figsize=(9, 5))

    for densidade, grupo in resumo.groupby("densidade"):
        grupo = grupo.sort_values("vertices")
        grupo = grupo.dropna(subset=[coluna])

        if grupo.empty:
            continue

        plt.plot(
            grupo["vertices"],
            grupo[coluna],
            marker="o",
            label=f"Densidade {densidade}%"
        )

    plt.xlabel("Numero de vertices")
    plt.ylabel(ylabel)
    plt.title(titulo)
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    plt.savefig(SAIDA / arquivo, dpi=200)
    plt.close()


def grafico_taxa_conclusao(resumo):
    plt.figure(figsize=(9, 5))

    for densidade, grupo in resumo.groupby("densidade"):
        grupo = grupo.sort_values("vertices")

        plt.plot(
            grupo["vertices"],
            grupo["taxa_conclusao"] * 100,
            marker="o",
            label=f"Densidade {densidade}%"
        )

    plt.xlabel("Numero de vertices")
    plt.ylabel("Buscas concluidas (%)")
    plt.title("Taxa de conclusao por tamanho do grafo")
    plt.ylim(-5, 105)
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()

    plt.savefig(SAIDA / "taxa_conclusao.png", dpi=200)
    plt.close()


def main():
    SAIDA.mkdir(parents=True, exist_ok=True)

    df = carregar_resultados()
    resumo = calcular_resumo(df)

    df.to_csv(
        SAIDA / "dados_consolidados.csv",
        index=False
    )

    resumo.to_csv(
        SAIDA / "resumo_final.csv",
        index=False
    )

    grafico_comparativo(
        resumo,
        "tempo_medio",
        "Tempo de execucao da busca completa",
        "Tempo medio (s)",
        "tempo_comparativo.png"
    )

    grafico_comparativo(
        resumo,
        "caminhos_medios",
        "Crescimento do numero de caminhos",
        "Caminhos explorados",
        "caminhos_comparativo.png"
    )

    grafico_comparativo(
        resumo,
        "chamadas_medias",
        "Crescimento das chamadas recursivas",
        "Chamadas recursivas",
        "chamadas_comparativo.png"
    )

    grafico_taxa_conclusao(resumo)

    print("\nResumo final:")
    print(resumo.to_string(index=False))

    print(f"\nArquivos salvos em: {SAIDA}")


if __name__ == "__main__":
    main()
