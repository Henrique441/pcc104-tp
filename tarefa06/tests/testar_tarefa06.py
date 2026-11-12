
import subprocess
from pathlib import Path

EXECUTAVEL = Path("./tarefa06.exe")
ARQUIVO_GRAFO = Path("tests/entrada01.gr")

def testar_backtracking():
    with open(ARQUIVO_GRAFO, "r") as arquivo:
        resultado = subprocess.run(
            [str(EXECUTAVEL.resolve()), "1", "5", "10"],
            stdin=arquivo,
            capture_output=True,
            text=True,
            timeout=15
        )

    print(resultado.stdout)

    if resultado.returncode != 0:
        print(resultado.stderr)
        raise RuntimeError("Erro na execucao do programa")

    assert "Melhor custo: 8" in resultado.stdout
    assert "Busca concluida: Sim" in resultado.stdout

    print("Teste aprovado!")

if __name__ == "__main__":
    testar_backtracking()
