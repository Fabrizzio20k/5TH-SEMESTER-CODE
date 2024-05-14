from sklearn.metrics import accuracy_score, confusion_matrix
from sklearn.tree import DecisionTreeClassifier
import h5py
import numpy as np

# Abre el archivo de entrenamiento en modo lectura
with h5py.File('train.h5', 'r') as f:
    # Imprime los nombres de los grupos/datasets en el archivo
    print("Keys: %s" % f.keys())
    a_group_key = list(f.keys())[0]
    b_group_key = list(f.keys())[1]

    # Accede a los datos del grupo/dataset
    data_train_X = list(f[a_group_key])
    data_train_Y = list(f[b_group_key])

    data_train_X = np.array(data_train_X)
    data_train_Y = np.array(data_train_Y)

    print(data_train_X.shape)
    print(data_train_Y.shape)

# Abre el archivo de prueba en modo lectura
with h5py.File('test.h5', 'r') as f:
    # Imprime los nombres de los grupos/datasets en el archivo
    print("Keys: %s" % f.keys())
    a_group_key = list(f.keys())[0]

    # Accede a los datos del grupo/dataset
    data_test_X = list(f[a_group_key])

    data_test_X = np.array(data_test_X)

    print(data_test_X.shape)

# Aplana los datos
data_train_X = data_train_X.reshape(data_train_X.shape[0], -1)
data_test_X = data_test_X.reshape(data_test_X.shape[0], -1)

# use decision tree, get accuracy and get confusion matrix

# Entrena el modelo
model = DecisionTreeClassifier()
model.fit(data_train_X, data_train_Y)

# Realiza predicciones
predictions = model.predict(data_test_X)

# Obtiene la exactitud
accuracy = accuracy_score(data_train_Y, predictions)
print("Accuracy: %.2f" % accuracy)
