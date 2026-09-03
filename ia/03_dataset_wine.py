
import json
import numpy as np
from sklearn.neighbors import KNeighborsClassifier
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import (
    accuracy_score, precision_score, recall_score, f1_score,
    confusion_matrix, classification_report
)

from data_prep import load_wine

RANDOM_STATE = 42


def evaluate(y_true, y_pred, label):
    acc = accuracy_score(y_true, y_pred)
    prec = precision_score(y_true, y_pred, average="binary", zero_division=0)
    rec = recall_score(y_true, y_pred, average="binary", zero_division=0)
    f1 = f1_score(y_true, y_pred, average="binary", zero_division=0)
    print(f"[{label}] acc={acc:.4f} prec={prec:.4f} rec={rec:.4f} f1={f1:.4f}")
    return {"accuracy": acc, "precision": prec, "recall": rec, "f1": f1}


def run():
    data = load_wine()
    X_train, X_test = data["X_train"], data["X_test"]
    y_train, y_test = data["y_train"], data["y_test"]
    class_names = data["class_names"]

    results = {"dataset": "Wine Quality (red+white)", "knn_variations": [], "logistic_regression": {}}

    # KNN
    # Hiperparâmetro variado: n_neighbors (k)
    print("\n===== KNN — variações de n_neighbors (k) =====")
    k_values = [3, 7, 15, 31]

    knn_models = {}
    for k in k_values:
        clf = KNeighborsClassifier(n_neighbors=k)
        clf.fit(X_train, y_train)
        y_pred = clf.predict(X_test)
        metrics = evaluate(y_test, y_pred, f"KNN k={k}")
        metrics["n_neighbors"] = k
        results["knn_variations"].append(metrics)
        knn_models[k] = (clf, y_pred)

    best_k = max(results["knn_variations"], key=lambda r: r["f1"])["n_neighbors"]
    best_knn_clf, best_knn_pred = knn_models[best_k]
    print(f"\nMelhor k: {best_k}")
    cm_knn = confusion_matrix(y_test, best_knn_pred)
    print("Matriz de confusão (melhor KNN):\n", cm_knn)
    report_knn = classification_report(y_test, best_knn_pred, target_names=class_names, zero_division=0)
    print(report_knn)

    results["knn_best"] = {
        "n_neighbors": best_k,
        "confusion_matrix": cm_knn.tolist(),
        "classification_report": report_knn,
    }

    #  REGRESSÃO LOGÍSTICA
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

    np.savez(
        "outputs/wine_predictions.npz",
        y_test=y_test, y_pred_knn=best_knn_pred, y_pred_lr=y_pred_lr,
        cm_knn=cm_knn, cm_lr=cm_lr,
    )

    with open("outputs/wine_results.json", "w") as f:
        json.dump(results, f, indent=2, default=str)

    print("\nResultados salvos em outputs/wine_results.json")
    return results, class_names


if __name__ == "__main__":
    run()
