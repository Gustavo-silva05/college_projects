
import pandas as pd
import numpy as np
from sklearn.preprocessing import LabelEncoder, StandardScaler
from sklearn.model_selection import train_test_split

RANDOM_STATE = 42

def load_obesity():
    df = pd.read_csv("ObesityDataSet_raw_and_data_sinthetic.csv")

    target_col = "NObeyesdad"
    y_raw = df[target_col]
    X = df.drop(columns=[target_col])

    # (7 classes ordinais: Insufficient -> Obesity_Type_III)
    class_order = [
        "Insufficient_Weight", "Normal_Weight",
        "Overweight_Level_I", "Overweight_Level_II",
        "Obesity_Type_I", "Obesity_Type_II", "Obesity_Type_III"
    ]
    le_target = LabelEncoder()
    le_target.fit(class_order)
    y = le_target.transform(y_raw)

    # Colunas categóricas (string) -> one-hot ; binárias yes/no -> 0/1
    binary_cols = ["Gender", "family_history_with_overweight", "FAVC", "SMOKE", "SCC"]
    multi_cat_cols = ["CAEC", "CALC", "MTRANS"]

    for col in binary_cols:
        X[col] = LabelEncoder().fit_transform(X[col])

    X = pd.get_dummies(X, columns=multi_cat_cols, drop_first=True)

    feature_names = X.columns.tolist()

    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=RANDOM_STATE, stratify=y
    )

    scaler = StandardScaler()
    X_train_scaled = scaler.fit_transform(X_train)
    X_test_scaled = scaler.transform(X_test)

    return {
        "X_train": X_train_scaled, "X_test": X_test_scaled,
        "X_train_raw": X_train.values, "X_test_raw": X_test.values,
        "y_train": y_train, "y_test": y_test,
        "feature_names": feature_names,
        "class_names": class_order,
        "le_target": le_target,
    }

def load_wine():
    red = pd.read_csv("winequality-red.csv", sep=";")
    white = pd.read_csv("winequality-white.csv", sep=";")

    red["wine_type"] = 0   # 0 = red
    white["wine_type"] = 1  # 1 = white

    df = pd.concat([red, white], axis=0, ignore_index=True)

    # Binarização do target: quality >= 6 -> 1 (bom), < 6 -> 0 (ruim)
    df["quality_label"] = (df["quality"] >= 6).astype(int)

    y = df["quality_label"].values
    X = df.drop(columns=["quality", "quality_label"])

    feature_names = X.columns.tolist()

    X_train, X_test, y_train, y_test = train_test_split(
        X, y, test_size=0.2, random_state=RANDOM_STATE, stratify=y
    )

    scaler = StandardScaler()
    X_train_scaled = scaler.fit_transform(X_train)
    X_test_scaled = scaler.transform(X_test)

    return {
        "X_train": X_train_scaled, "X_test": X_test_scaled,
        "X_train_raw": X_train.values, "X_test_raw": X_test.values,
        "y_train": y_train, "y_test": y_test,
        "feature_names": feature_names,
        "class_names": ["Ruim (<6)", "Bom (>=6)"],
    }


if __name__ == "__main__":
    obesity = load_obesity()
    wine = load_wine()

    print("=== OBESITY ===")
    print("Treino:", obesity["X_train"].shape, "Teste:", obesity["X_test"].shape)
    print("Classes:", obesity["class_names"])
    print("Distribuição treino:", np.bincount(obesity["y_train"]))

    print("\n=== WINE ===")
    print("Treino:", wine["X_train"].shape, "Teste:", wine["X_test"].shape)
    print("Distribuição treino:", np.bincount(wine["y_train"]))
    print("Proporção red/white:", np.bincount(wine["X_train_raw"][:, -1].astype(int)))
