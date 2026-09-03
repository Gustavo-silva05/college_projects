import os
import json
import numpy as np
import pandas as pd
from sklearn.naive_bayes import GaussianNB
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import (
    accuracy_score, precision_score, recall_score, f1_score,
    confusion_matrix, classification_report
)

from data_prep import load_obesity

RANDOM_STATE = 42


def evaluate(y_true, y_pred, label):
    acc = accuracy_score(y_true, y_pred)
    prec = precision_score(y_true, y_pred, average="macro", zero_division=0)
    rec = recall_score(y_true, y_pred, average="macro", zero_division=0)
    f1 = f1_score(y_true, y_pred, average="macro", zero_division=0)
    print(f"[{label}] acc={acc:.4f} prec_macro={prec:.4f} rec_macro={rec:.4f} f1_macro={f1:.4f}")
    return {"accuracy": acc, "precision_macro": prec, "recall_macro": rec, "f1_macro": f1}


def run():
    data = load_obesity()
    X_train, X_test = data["X_train"], data["X_test"]
    y_train, y_test = data["y_train"], data["y_test"]
    class_names = data["class_names"]

    results = {"dataset": "Obesity", "naive_bayes_variations": [], "logistic_regression": {}}

    #  NAIVE BAYES (GaussianNB)
    # Hiperparâmetro variado: var_smoothing
    print("\n===== NAIVE BAYES (GaussianNB) — variações de var_smoothing =====")
    var_smoothing_values = [1e-9, 1e-5, 1e-2, 1e-1]

    nb_models = {}
    for vs in var_smoothing_values:
        clf = GaussianNB(var_smoothing=vs)
        clf.fit(X_train, y_train)
        y_pred = clf.predict(X_test)
        metrics = evaluate(y_test, y_pred, f"NB var_smoothing={vs}")
        metrics["var_smoothing"] = vs
        results["naive_bayes_variations"].append(metrics)
        nb_models[vs] = (clf, y_pred)

    # Melhor NB por f1_macro
    best_vs = max(results["naive_bayes_variations"], key=lambda r: r["f1_macro"])["var_smoothing"]
    best_nb_clf, best_nb_pred = nb_models[best_vs]
    print(f"\nMelhor var_smoothing: {best_vs}")
    cm_nb = confusion_matrix(y_test, best_nb_pred)
    print("Matriz de confusão (melhor NB):\n", cm_nb)
    report_nb = classification_report(y_test, best_nb_pred, target_names=class_names, zero_division=0)
    print(report_nb)

    results["naive_bayes_best"] = {
        "var_smoothing": best_vs,
        "confusion_matrix": cm_nb.tolist(),
        "classification_report": report_nb,
    }

    # MODELO ESCOLHIDO: REGRESSÃO LOGÍSTICA
    print("\n===== REGRESSÃO LOGÍSTICA =====")
    lr = LogisticRegression(max_iter=2000, random_state=RANDOM_STATE)
    lr.fit(X_train, y_train)
    y_pred_lr = lr.predict(X_test)
    metrics_lr = evaluate(y_test, y_pred_lr, "LogReg")
    cm_lr = confusion_matrix(y_test, y_pred_lr)
    report_lr = classification_report(y_test, y_pred_lr, target_names=class_names, zero_division=0)
    print("Matriz de confusão (LogReg):\n", cm_lr)
    print(report_lr)

    results["logistic_regression"] = {
        **metrics_lr,
        "confusion_matrix": cm_lr.tolist(),
        "classification_report": report_lr,
    }

    base_dir = os.path.dirname(os.path.abspath(__file__))
    outputs_dir = os.path.join(base_dir, "outputs")
    os.makedirs(outputs_dir, exist_ok=True)

    np.savez(
        os.path.join(outputs_dir, "obesity_predictions.npz"),
        y_test=y_test, y_pred_nb=best_nb_pred, y_pred_lr=y_pred_lr,
        cm_nb=cm_nb, cm_lr=cm_lr,
    )

    with open(os.path.join(outputs_dir, "obesity_results.json"), "w") as f:
        json.dump(results, f, indent=2, default=str)

    print("\nResultados salvos em outputs/obesity_results.json")
    return results, class_names


if __name__ == "__main__":
    run()
