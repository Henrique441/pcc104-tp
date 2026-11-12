
import pandas as pd

entrada = "resultados/finais/resumo_experimentos.csv"
saida = "resultados/finais/comparacao_final.csv"

df = pd.read_csv(entrada)

registros = []

for instancia, grupo in df.groupby("instancia"):
    dados = grupo.set_index("modo")

    if not {0, 1, 2}.issubset(dados.index):
        continue

    completa = dados.loc[0]
    poda1 = dados.loc[1]
    poda2 = dados.loc[2]

    def reducao(valor_base, valor_novo):
        if valor_base == 0:
            return 0.0
        return 100 * (1 - valor_novo / valor_base)

    registros.append({
        "instancia": instancia,
        "vertices": completa["vertices"],
        "densidade": completa["densidade"],

        "tempo_completa": completa["tempo_medio"],
        "tempo_poda1": poda1["tempo_medio"],
        "tempo_poda2": poda2["tempo_medio"],

        "chamadas_completa": completa["chamadas_medias"],
        "chamadas_poda1": poda1["chamadas_medias"],
        "chamadas_poda2": poda2["chamadas_medias"],

        "podas_poda1": poda1["podas_medias"],
        "podas_poda2": poda2["podas_medias"],

        "reducao_poda1_pct": reducao(
            completa["chamadas_medias"],
            poda1["chamadas_medias"]
        ),
        "reducao_poda2_pct": reducao(
            completa["chamadas_medias"],
            poda2["chamadas_medias"]
        ),

        "conclusao_completa": completa["taxa_conclusao"],
        "conclusao_poda1": poda1["taxa_conclusao"],
        "conclusao_poda2": poda2["taxa_conclusao"],

        "custos_completa": completa["custos_observados"],
        "custos_poda1": poda1["custos_observados"],
        "custos_poda2": poda2["custos_observados"]
    })

comparacao = pd.DataFrame(registros)

comparacao = comparacao.sort_values(
    ["densidade", "vertices"]
)

comparacao.to_csv(saida, index=False)

print(comparacao[[
    "vertices",
    "densidade",
    "reducao_poda1_pct",
    "reducao_poda2_pct"
]].round(2).to_string(index=False))

print(f"\nArquivo salvo em: {saida}")
