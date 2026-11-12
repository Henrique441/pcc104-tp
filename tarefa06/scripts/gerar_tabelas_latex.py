
import csv
from pathlib import Path

ENTRADA = Path("resultados/finais/comparacao_final.csv")
SAIDA = Path("resultados/finais/tabelas.tex")


def numero(valor, casas=2):
    return f"{float(valor):.{casas}f}".replace(".", ",")


def tempo(valor):
    segundos = float(valor)

    if segundos < 0.001:
        return f"{numero(segundos * 1_000_000, 2)} $\\mu$s"

    if segundos < 1:
        return f"{numero(segundos * 1000, 2)} ms"

    return f"{numero(segundos, 3)} s"


def tabela_tempos(dados):
    linhas = [
        r"\begin{table}[htbp]",
        r"\centering",
        r"\caption{Tempo médio de execução dos algoritmos}",
        r"\label{tab:tempos_tarefa6}",
        r"\small",
        r"\begin{tabular}{ccccc}",
        r"\hline",
        r"V & Densidade & Completa & Poda 1 & Podas 1+2 \\",
        r"\hline"
    ]

    for item in dados:
        v = int(float(item["vertices"]))
        d = round(float(item["densidade"]) * 100)

        t0 = tempo(item["tempo_completa"])
        t1 = tempo(item["tempo_poda1"])
        t2 = tempo(item["tempo_poda2"])

        linha = (
            f"{v} & {d}\\% & {t0} & "
            f"{t1} & {t2} " + r"\\"
        )
        linhas.append(linha)

    linhas.extend([
        r"\hline",
        r"\end{tabular}",
        r"\end{table}"
    ])

    return "\n".join(linhas)


def tabela_reducoes(dados):
    linhas = [
        r"\begin{table}[htbp]",
        r"\centering",
        r"\caption{Redução das chamadas recursivas}",
        r"\label{tab:reducoes_tarefa6}",
        r"\small",
        r"\begin{tabular}{cccc}",
        r"\hline",
        r"V & Densidade & Poda 1 & Podas 1+2 \\",
        r"\hline"
    ]

    for item in dados:
        v = int(float(item["vertices"]))
        d = round(float(item["densidade"]) * 100)

        r1 = numero(item["reducao_poda1_pct"], 4)
        r2 = numero(item["reducao_poda2_pct"], 4)

        linhas.append(
            f"{v} & {d}\\% & {r1}\\% & "
            f"{r2}\\% " + r"\\"
        )

    linhas.extend([
        r"\hline",
        r"\end{tabular}",
        r"\end{table}"
    ])

    return "\n".join(linhas)


def main():
    with open(ENTRADA, encoding="utf-8") as arquivo:
        dados = list(csv.DictReader(arquivo))

    dados.sort(
        key=lambda item: (
            float(item["densidade"]),
            int(float(item["vertices"]))
        )
    )

    conteudo = (
        "% Tabelas geradas automaticamente\n\n"
        + tabela_tempos(dados)
        + "\n\n"
        + tabela_reducoes(dados)
        + "\n"
    )

    SAIDA.parent.mkdir(parents=True, exist_ok=True)
    SAIDA.write_text(conteudo, encoding="utf-8")

    print(f"Tabelas geradas em: {SAIDA}")


if __name__ == "__main__":
    main()
