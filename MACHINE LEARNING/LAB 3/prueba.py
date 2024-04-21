import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split


def h(x, w):
    return np.dot(x, w)


def S(x, w):
    z = h(x, w)
    return 1 / (1 + np.exp(-z))


def Loss(y, y_aprox):
    e = 1e-10
    y_aprox = np.clip(y_aprox, e, 1 - e)
    return -np.mean(y * np.log(y_aprox) + (1 - y) * np.log(1 - y_aprox))


def Derivatives(x, y, w):
    m = len(y)
    s_aprox = S(x, w)
    e = y - s_aprox
    dw = (1/m) * np.dot(x.T, e)
    return dw


def change_parameters(w, derivatives, alpha):
    return w - alpha * derivatives


def training(x, y, epochs, alpha):
    L_value = []
    n_features = x.shape[1]
    w = np.zeros(n_features)
    for i in range(epochs):
        y_aprox = S(x, w)
        L = Loss(y, y_aprox)
        dw = Derivatives(x, y, w)
        w = change_parameters(w, dw, alpha)
        L_value.append(L)
    return L_value, w


def Testing(x_test, y_test, w):
    n = len(y_test)
    y_pred = []
    for i in range(n):
        y_pred.append(S(x_test, w))
    print("Number of correct data : ", sum(y_pred == y_test))


data = pd.read_csv('db.csv')
X = data[["C1", "C2", "C3"]]
Y = data["Clase"]

X = (X - X.min()) / (X.max() - X.min())

X_train, X_test, Y_train, Y_test = train_test_split(
    X, Y, random_state=104, test_size=0.30, shuffle=True)

epochs = 1000
alpha = 0.001

L, W = training(X_train.to_numpy(), Y_train.to_numpy(), epochs, alpha)

plt.plot(range(epochs), L)
plt.xlabel("Epochs")
plt.ylabel("Loss")
plt.title("Loss vs Epochs")
plt.show()

Testing(X_test.to_numpy(), Y_test.to_numpy(), W)
