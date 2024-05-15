#!/usr/bin/env python3

import pandas as pd
import h5py
import matplotlib.pyplot as plt
import numpy as np

from tsfresh import extract_features
from tsfresh.feature_extraction import EfficientFCParameters

with h5py.File("train.h5", "r") as f:
    X = f["x"][:]
    Y = f["y"][:]
    X = np.array([i[0] for i in X])

print(type(Y))
print(X.shape)
print(Y.shape)

print(X)

select_features = 100

# Reduce features
from sklearn.decomposition import PCA

pca = PCA(n_components=select_features)
X_pca = pca.fit_transform(X)
print(X_pca.shape)

# Convierte X a un DataFrame de pandas
df_X = pd.DataFrame(X_pca.reshape(-1, X_pca.shape[-1]))


# Convierte Y a un DataFrame de pandas
df_Y = pd.DataFrame(Y, columns=["Target"])

# Une los dos DataFrames
df = pd.concat([df_X, df_Y], axis=1)

print(df)


df_tsfresh = pd.DataFrame(
    {
        "id": np.repeat(df.index, select_features),
        "time": np.tile(np.arange(select_features), len(df)),
        "value": df.drop(columns="Target").values.flatten(),
    }
)
print(df_tsfresh)
# print quantity of zeros and ones
# print(df_Y["Target"].value_counts())

features = extract_features(
    df_tsfresh,
    column_id="id",
    column_sort="time",
    n_jobs=24,
    default_fc_parameters=EfficientFCParameters(),
)
print(features)


from sklearn.tree import DecisionTreeClassifier
from sklearn.metrics import classification_report
from sklearn.model_selection import train_test_split

# Asume que 'features' es tu DataFrame con las características extraídas y 'df_Y' es tu DataFrame con las etiquetas
X = features
y = df_Y["Target"]

# Divide tus datos en conjuntos de entrenamiento y prueba
X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.3, random_state=42
)
X_train = X_train.fillna(0)
X_test = X_test.fillna(0)

clf = DecisionTreeClassifier()

# Entrena el clasificador
clf.fit(X_train, y_train)

# Haz predicciones en el conjunto de prueba
y_pred = clf.predict(X_test)

# Imprime un informe de clasificación
print(classification_report(y_test, y_pred))

# get accuracy
from sklearn.metrics import accuracy_score

accuracy = accuracy_score(y_test, y_pred)
print(accuracy)

# create confusion matrix
from sklearn.metrics import confusion_matrix

# plot confusion matrix
import seaborn as sns

sns.heatmap(confusion_matrix(y_test, y_pred), annot=True, fmt="d", cmap="Blues")
plt.xlabel("Predicted")
plt.ylabel("Actual")
plt.show()
