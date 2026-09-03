import json
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

plt.rcParams.update({"font.size": 10, "figure.dpi": 150})

OUT = "outputs"

with open(f"{OUT}/obesity_results.json") as f:
    ob = json.load(f)
with open(f"{OUT}/wine_results.json") as f:
    wn = json.load(f)

ob_npz = np.load(f"{OUT}/obesity_predictions.npz")
wn_npz = np.load(f"{OUT}/wine_predictions.npz")

obesity_classes = [
    "Insuf.", "Normal", "Over.I", "Over.II", "Obes.I", "Obes.II", "Obes.III"
]
wine_classes = ["Ruim", "Bom"]

# GRÁFICO 1 — Naive Bayes: impacto do var_smoothing (Obesity)
nb_var = ob["naive_bayes_variations"]
vs_vals = [r["var_smoothing"] for r in nb_var]
vs_acc = [r["accuracy"] for r in nb_var]
vs_f1 = [r["f1_macro"] for r in nb_var]

fig, ax = plt.subplots(figsize=(6, 4))
x = np.arange(len(vs_vals))
ax.plot(x, vs_acc, marker="o", label="Acurácia", linewidth=2)
ax.plot(x, vs_f1, marker="s", label="F1-macro", linewidth=2)
ax.set_xticks(x)
ax.set_xticklabels([f"{v:.0e}" for v in vs_vals])
ax.set_xlabel("var_smoothing")
ax.set_ylabel("Score")
ax.set_title("Naive Bayes (Obesity) — Impacto do var_smoothing")
ax.legend()
ax.grid(alpha=0.3)
plt.tight_layout()
plt.savefig(f"{OUT}/fig1_nb_hyperparam.png")
plt.close()

# GRÁFICO 2 — KNN: impacto do k (Wine)
knn_var = wn["knn_variations"]
k_vals = [r["n_neighbors"] for r in knn_var]
k_acc = [r["accuracy"] for r in knn_var]
k_f1 = [r["f1"] for r in knn_var]

fig, ax = plt.subplots(figsize=(6, 4))
ax.plot(k_vals, k_acc, marker="o", label="Acurácia", linewidth=2)
ax.plot(k_vals, k_f1, marker="s", label="F1 (classe Bom)", linewidth=2)
ax.set_xlabel("k (n_neighbors)")
ax.set_ylabel("Score")
ax.set_title("KNN (Wine Quality) — Impacto de k")
ax.legend()
ax.grid(alpha=0.3)
plt.tight_layout()
plt.savefig(f"{OUT}/fig2_knn_hyperparam.png")
plt.close()

# GRÁFICO 3 — Matrizes de confusão (4 painéis)
fig, axes = plt.subplots(2, 2, figsize=(11, 10))

sns.heatmap(ob_npz["cm_nb"], annot=True, fmt="d", cmap="Blues", ax=axes[0, 0],
            xticklabels=obesity_classes, yticklabels=obesity_classes, cbar=False)
axes[0, 0].set_title("Obesity — Naive Bayes")
axes[0, 0].set_xlabel("Predito"); axes[0, 0].set_ylabel("Real")

sns.heatmap(ob_npz["cm_lr"], annot=True, fmt="d", cmap="Greens", ax=axes[0, 1],
            xticklabels=obesity_classes, yticklabels=obesity_classes, cbar=False)
axes[0, 1].set_title("Obesity — Regressão Logística")
axes[0, 1].set_xlabel("Predito"); axes[0, 1].set_ylabel("Real")

sns.heatmap(wn_npz["cm_knn"], annot=True, fmt="d", cmap="Blues", ax=axes[1, 0],
            xticklabels=wine_classes, yticklabels=wine_classes, cbar=False)
axes[1, 0].set_title("Wine — KNN")
axes[1, 0].set_xlabel("Predito"); axes[1, 0].set_ylabel("Real")

sns.heatmap(wn_npz["cm_lr"], annot=True, fmt="d", cmap="Greens", ax=axes[1, 1],
            xticklabels=wine_classes, yticklabels=wine_classes, cbar=False)
axes[1, 1].set_title("Wine — Regressão Logística")
axes[1, 1].set_xlabel("Predito"); axes[1, 1].set_ylabel("Real")

plt.tight_layout()
plt.savefig(f"{OUT}/fig3_confusion_matrices.png")
plt.close()

# GRÁFICO 4 — Comparação geral de métricas entre os 4 modelos
models = ["NB\n(Obesity)", "LogReg\n(Obesity)", "KNN\n(Wine)", "LogReg\n(Wine)"]
acc_values = [
    ob["naive_bayes_variations"][2]["accuracy"],  # melhor NB (var_smoothing=0.01)
    ob["logistic_regression"]["accuracy"],
    wn["knn_variations"][1]["accuracy"],  # melhor KNN (k=7)
    wn["logistic_regression"]["accuracy"],
]
f1_values = [
    ob["naive_bayes_variations"][2]["f1_macro"],
    ob["logistic_regression"]["f1_macro"],
    wn["knn_variations"][1]["f1"],
    wn["logistic_regression"]["f1"],
]

fig, ax = plt.subplots(figsize=(8, 5))
x = np.arange(len(models))
width = 0.35
bars1 = ax.bar(x - width/2, acc_values, width, label="Acurácia", color="#4C72B0")
bars2 = ax.bar(x + width/2, f1_values, width, label="F1", color="#DD8452")
ax.set_xticks(x)
ax.set_xticklabels(models)
ax.set_ylabel("Score")
ax.set_title("Comparação entre os 4 modelos aplicados")
ax.set_ylim(0, 1.05)
ax.legend()
ax.grid(alpha=0.3, axis="y")
for bars in (bars1, bars2):
    for b in bars:
        ax.annotate(f"{b.get_height():.2f}", (b.get_x() + b.get_width()/2, b.get_height()),
                    ha="center", va="bottom", fontsize=8)
plt.tight_layout()
plt.savefig(f"{OUT}/fig4_model_comparison.png")
plt.close()

print("Gráficos gerados em outputs/:")
print(" - fig1_nb_hyperparam.png")
print(" - fig2_knn_hyperparam.png")
print(" - fig3_confusion_matrices.png")
print(" - fig4_model_comparison.png")
