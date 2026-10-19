
#!/usr/bin/env bash

set -euo pipefail

PASTA="tests/desempenho"
REPETICOES=10
DIMENSOES=(8 16 32 64 128)

mkdir -p "$PASTA"

python - "$REPETICOES" "${DIMENSOES[@]}" <<'PY'
import csv
import subprocess
import sys
import time
from pathlib import Path

repeticoes = int(sys.argv[1])
dimensoes = [int(n) for n in sys.argv[2:]]

pasta = Path("tests/desempenho")
executavel = Path("tarefa01.exe").resolve()
arquivo_csv = pasta / "resultados.csv"

if not executavel.is_file():
    raise SystemExit("Executavel nao encontrado. Execute make.")

with arquivo_csv.open("w", newline="", encoding="utf-8") as arquivo:
    escritor = csv.writer(arquivo)
    escritor.writerow(["n", "tempo_medio_s"])

    for n in dimensoes:
        entrada = pasta / f"entrada{n:02d}.txt"

        if not entrada.is_file():
            raise SystemExit(f"Arquivo nao encontrado: {entrada}")

        tempos = []

        for i in range(repeticoes):
            with entrada.open("rb") as dados:
                inicio = time.perf_counter()

                subprocess.run(
                    [str(executavel)],
                    stdin=dados,
                    stdout=subprocess.DEVNULL,
                    check=True
                )

                fim = time.perf_counter()

            tempos.append(fim - inicio)

        media = sum(tempos) / repeticoes
        escritor.writerow([n, f"{media:.6f}"])

        print(
            f"n={n:3d} | "
            f"tempo medio={media:.6f} s | "
            f"repeticoes={repeticoes}"
        )

print(f"\nResultados salvos em {arquivo_csv}")
PY
