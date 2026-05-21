import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
import os

# ── Configurações ──────────────────────────────────────────────────────────────
CSV = "resultados_consolidados.csv"
OUTPUT_DIR = "graficos"
os.makedirs(OUTPUT_DIR, exist_ok=True)

ALGORITMOS = ["FIFO", "SJF", "LJF", "PRIO_STATIC", "PRIO_DYNAMIC", "PRIO_DYNAMIC_QUANTUM"]
QUANTUMS   = [10, 20, 30]

# Paleta de cores consistente por algoritmo
CORES = {
    "FIFO":                 "#4C9BE8",
    "SJF":                  "#2ECC71",
    "LJF":                  "#E74C3C",
    "PRIO_STATIC":          "#F39C12",
    "PRIO_DYNAMIC":         "#9B59B6",
    "PRIO_DYNAMIC_QUANTUM": "#1ABC9C",
}

MARCADORES = {
    "FIFO":                 "o",
    "SJF":                  "s",
    "LJF":                  "^",
    "PRIO_STATIC":          "D",
    "PRIO_DYNAMIC":         "v",
    "PRIO_DYNAMIC_QUANTUM": "P",
}

# ── Leitura dos dados ──────────────────────────────────────────────────────────
df = pd.read_csv(CSV)
print(f"CSV carregado: {len(df)} linhas")

# ── Gráfico 1: TME por algoritmo para cada quantum (com barras de erro) ───────
for q in QUANTUMS:
    fig, ax = plt.subplots(figsize=(10, 6))
    df_q = df[df["Quantum"] == q].sort_values("NPROC")

    for alg in ALGORITMOS:
        df_alg = df_q[df_q["Algoritmo"] == alg]
        if df_alg.empty:
            continue
        ax.errorbar(
            df_alg["NPROC"],
            df_alg["TME_Media"],
            yerr=df_alg["TME_Desvio_Padrao"],
            label=alg,
            color=CORES[alg],
            marker=MARCADORES[alg],
            linewidth=2,
            markersize=6,
            capsize=4,
            alpha=0.85,
        )

    ax.set_title(f"Tempo Médio de Espera por Algoritmo (Quantum = {q} ms)", fontsize=13, fontweight="bold")
    ax.set_xlabel("Número de Processos (N)", fontsize=11)
    ax.set_ylabel("TME Médio (µs)", fontsize=11)
    ax.yaxis.set_major_formatter(ticker.FuncFormatter(lambda x, _: f"{x:,.0f}"))
    ax.legend(title="Algoritmo", fontsize=9, title_fontsize=10)
    ax.grid(True, linestyle="--", alpha=0.5)
    ax.set_xticks(sorted(df["NPROC"].unique()))
    plt.tight_layout()

    fname = os.path.join(OUTPUT_DIR, f"tme_por_algoritmo_Q{q}.png")
    plt.savefig(fname, dpi=150)
    plt.close()
    print(f"Salvo: {fname}")

# ── Gráfico 2: Impacto do quantum por algoritmo ───────────────────────────────
for alg in ALGORITMOS:
    fig, ax = plt.subplots(figsize=(10, 6))
    df_alg = df[df["Algoritmo"] == alg].sort_values("NPROC")

    for q in QUANTUMS:
        df_q = df_alg[df_alg["Quantum"] == q]
        if df_q.empty:
            continue
        ax.plot(
            df_q["NPROC"],
            df_q["TME_Media"],
            label=f"Q = {q} ms",
            marker="o",
            linewidth=2,
            markersize=6,
        )

    ax.set_title(f"Impacto do Quantum no TME — {alg}", fontsize=13, fontweight="bold")
    ax.set_xlabel("Número de Processos (N)", fontsize=11)
    ax.set_ylabel("TME Médio (µs)", fontsize=11)
    ax.yaxis.set_major_formatter(ticker.FuncFormatter(lambda x, _: f"{x:,.0f}"))
    ax.legend(title="Quantum", fontsize=9, title_fontsize=10)
    ax.grid(True, linestyle="--", alpha=0.5)
    ax.set_xticks(sorted(df["NPROC"].unique()))
    plt.tight_layout()

    fname = os.path.join(OUTPUT_DIR, f"impacto_quantum_{alg}.png")
    plt.savefig(fname, dpi=150)
    plt.close()
    print(f"Salvo: {fname}")

# ── Gráfico 3: Comparação geral (média de todos os quantums) ──────────────────
fig, ax = plt.subplots(figsize=(11, 6))
df_media_q = df.groupby(["Algoritmo", "NPROC"])["TME_Media"].mean().reset_index()

for alg in ALGORITMOS:
    df_alg = df_media_q[df_media_q["Algoritmo"] == alg].sort_values("NPROC")
    if df_alg.empty:
        continue
    ax.plot(
        df_alg["NPROC"],
        df_alg["TME_Media"],
        label=alg,
        color=CORES[alg],
        marker=MARCADORES[alg],
        linewidth=2.5,
        markersize=7,
    )

ax.set_title("Comparação Geral — TME Médio por Algoritmo (média dos quantums)", fontsize=13, fontweight="bold")
ax.set_xlabel("Número de Processos (N)", fontsize=11)
ax.set_ylabel("TME Médio (µs)", fontsize=11)
ax.yaxis.set_major_formatter(ticker.FuncFormatter(lambda x, _: f"{x:,.0f}"))
ax.legend(title="Algoritmo", fontsize=9, title_fontsize=10)
ax.grid(True, linestyle="--", alpha=0.5)
ax.set_xticks(sorted(df["NPROC"].unique()))
plt.tight_layout()

fname = os.path.join(OUTPUT_DIR, "comparacao_geral.png")
plt.savefig(fname, dpi=150)
plt.close()
print(f"Salvo: {fname}")

print(f"\nTodos os gráficos foram salvos na pasta '{OUTPUT_DIR}/'")
