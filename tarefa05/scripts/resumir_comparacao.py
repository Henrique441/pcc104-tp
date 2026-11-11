
from pathlib import Path
import pandas as pd


entrada = Path(
    "resultados/comparacao_final/comparacao_completa.csv"
)

saida = Path(
    "resultados/comparacao_final/resumo_comparacao.csv"
)

df = pd.read_csv(entrada)

df["concluida"] = (
    df["concluida"]
    .astype(str)
    .str.lower()
    .eq("true")
)

df["custos_coincidem"] = (
    df["custo_busca"] == df["custo_dijkstra"]
)

df["resultado_validado"] = (
    df["concluida"] & df["custos_coincidem"]
)

resumo = (
    df.groupby(["densidade", "vertices"])
    .agg(
        repeticoes=("concluida", "size"),
        concluidas=("concluida", "sum"),
        taxa_conclusao=("concluida", "mean"),
        tempo_dijkstra_medio=("tempo_dijkstra", "mean"),
        caminhos_medios=("caminhos", "mean"),
        chamadas_medias=("chamadas", "mean"),
        validacoes=("resultado_validado", "sum")
    )
    .reset_index()
)

tempos_concluidos = (
    df[df["concluida"]]
    .groupby(["densidade", "vertices"])
    .agg(
        tempo_busca_medio=("tempo_busca", "mean"),
        tempo_busca_desvio=("tempo_busca", "std")
    )
    .reset_index()
)

resumo = resumo.merge(
    tempos_concluidos,
    on=["densidade", "vertices"],
    how="left"
)

resumo.to_csv(saida, index=False)

print(resumo.to_string(index=False))
print(f"\nArquivo salvo em: {saida}")
