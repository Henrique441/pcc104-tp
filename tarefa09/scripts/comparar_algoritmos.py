#!/usr/bin/env python3
"""Compara Floyd-Warshall (todos os pares) com Dijkstra (uma vez por origem)."""
import argparse
import csv
import random
import re
import subprocess
from pathlib import Path
from statistics import mean, stdev

PADRAO_CAMINHO = re.compile(r'^\[(\d+)\s*,\s*(\d+)\]\((INF|-?\d+)\)')
PADRAO_TEMPO = re.compile(r'Tempo de execu[cç][aã]o:\s*([0-9]+(?:\.[0-9]+)?)\s*(?:segundos|s)', re.I)


def executar(exe, grafo, argumentos=()):
    with grafo.open('rb') as entrada:
        p = subprocess.run([str(exe), *map(str, argumentos)], stdin=entrada,
                           stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                           text=True, check=True)
    m = PADRAO_TEMPO.search(p.stderr)
    if not m:
        raise ValueError(f'Tempo ausente em {exe}: {p.stderr}')
    return p.stdout, float(m.group(1))


def cabecalho(grafo):
    with grafo.open(encoding='utf-8') as f:
        for linha in f:
            p = linha.split()
            if len(p) == 4 and p[:2] == ['p', 'sp']:
                return int(p[2]), int(p[3])
    raise ValueError(f'Cabecalho DIMACS ausente: {grafo}')


def matriz_floyd(saida, n):
    matriz = {}
    for linha in saida.splitlines():
        p = linha.split()
        if len(p) != n + 1 or not p[0].isdigit():
            continue
        i = int(p[0])
        if 1 <= i <= n:
            try:
                matriz[i] = [None if x == 'INF' else int(x) for x in p[1:]]
            except ValueError:
                continue
    if set(matriz) != set(range(1, n + 1)):
        raise ValueError(f'Matriz Floyd incompleta: {len(matriz)}/{n}')
    return matriz


def distancias_dijkstra(saida, origem, n):
    dist = {origem: 0}
    for linha in saida.splitlines():
        m = PADRAO_CAMINHO.match(linha.strip())
        if m:
            o, d, custo = int(m.group(1)), int(m.group(2)), m.group(3)
            if o != origem or not 1 <= d <= n or d in dist:
                raise ValueError(f'Linha inesperada ou repetida: {linha}')
            dist[d] = None if custo == 'INF' else int(custo)
    if len(dist) != n:
        raise ValueError(f'Dijkstra origem {origem}: {len(dist)}/{n} distancias')
    return dist


def media_desvio(valores):
    return mean(valores), stdev(valores) if len(valores) > 1 else 0.0


def comparar(grafo, floyd_exe, dijkstra_exe, repeticoes, rng):
    n, e = cabecalho(grafo)
    saida_f, _ = executar(floyd_exe, grafo)
    matriz = matriz_floyd(saida_f, n)
    divergencias = []
    for origem in range(1, n + 1):
        saida_d, _ = executar(dijkstra_exe, grafo, (origem,))
        dist = distancias_dijkstra(saida_d, origem, n)
        for destino in range(1, n + 1):
            esperado, obtido = matriz[origem][destino - 1], dist[destino]
            if esperado != obtido:
                divergencias.append((origem, destino, esperado, obtido))
    if divergencias:
        raise AssertionError(f'{grafo}: {len(divergencias)} divergencias; primeiras: {divergencias[:5]}')

    tempos_f, tempos_d = [], []
    for _ in range(repeticoes):
        # Alterna a ordem das abordagens para reduzir viés sistematico.
        ordem = ['f', 'd']
        rng.shuffle(ordem)
        for algoritmo in ordem:
            if algoritmo == 'f':
                _, t = executar(floyd_exe, grafo)
                tempos_f.append(t)
            else:
                total = 0.0
                for origem in range(1, n + 1):
                    _, t = executar(dijkstra_exe, grafo, (origem,))
                    total += t
                tempos_d.append(total)

    mf, sf = media_desvio(tempos_f)
    md, sd = media_desvio(tempos_d)
    return dict(instancia=str(grafo), vertices=n, arestas=e,
                pares_validados=n*n, divergencias=0, repeticoes=repeticoes,
                tempo_medio_floyd_s=mf, desvio_floyd_s=sf,
                tempo_medio_dijkstra_todas_origens_s=md, desvio_dijkstra_s=sd,
                razao_dijkstra_floyd=(md/mf if mf > 0 else ''))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--pasta', type=Path, default=Path('../tarefa08/tests/instancias_sinteticas'))
    ap.add_argument('--floyd', type=Path, default=Path('./tarefa09.exe'))
    ap.add_argument('--dijkstra', type=Path, default=Path('../tarefa04/tarefa04.exe'))
    ap.add_argument('--saida', type=Path, default=Path('resultados/comparacao_corrigida.csv'))
    ap.add_argument('--repeticoes', type=int, default=30)
    args = ap.parse_args()
    if args.repeticoes < 1:
        ap.error('--repeticoes deve ser >= 1')
    floyd, dijkstra = args.floyd.resolve(), args.dijkstra.resolve()
    for exe in (floyd, dijkstra):
        if not exe.is_file():
            ap.error(f'Executavel nao encontrado: {exe}')
    grafos = sorted(args.pasta.rglob('*.gr'))
    if not grafos:
        ap.error(f'Nenhum .gr em {args.pasta}')
    args.saida.parent.mkdir(parents=True, exist_ok=True)
    rng = random.Random(42)
    with args.saida.open('w', newline='', encoding='utf-8') as arq:
        writer = None
        for grafo in grafos:
            r = comparar(grafo, floyd, dijkstra, args.repeticoes, rng)
            if writer is None:
                writer = csv.DictWriter(arq, fieldnames=list(r))
                writer.writeheader()
            writer.writerow(r)
            arq.flush()
            print(f'{grafo.parent.name}/{grafo.name}: OK ({r["pares_validados"]} pares) | '
                  f'Floyd={r["tempo_medio_floyd_s"]:.9f}s | '
                  f'Dijkstra V origens={r["tempo_medio_dijkstra_todas_origens_s"]:.9f}s')
    print(f'\nCSV salvo em: {args.saida}')
    print('Nota: tempos internos dos executaveis com resolucao de 1 microssegundo; '
          'interpretar valores muito pequenos com cautela.')


if __name__ == '__main__':
    main()
