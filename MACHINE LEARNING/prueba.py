import numpy as np
import matplotlib.pyplot as ptl
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
import pandas as pd
import category_encoders as ce


class LinearRegresion:
    def __init__(self, dim):
        self.dim = dim
        self.m_W = np.random.rand(dim)
        self.m_b = np.random.random()

    def H(self, X):
        return np.dot(X, self.m_W) + self.m_b

    def Loss_L2(self, X, Y, lambda_):
        y_pred = self.H(X)
        return (np.linalg.norm((Y - y_pred))**2)/(2*len(Y)), y_pred + lambda_*np.linalg.norm(self.m_W, 2)

    def Loss_L1(self, X, Y, lambda_):
        y_pred = self.H(X)
        return (np.linalg.norm((Y - y_pred))**2) / (2 * len(Y)), y_pred + lambda_ * np.linalg.norm(self.m_W, 1)

    def dL(self, X, Y, Y_pre):
        dw = np.matmul(Y - Y_pre, -X)/len(Y)
        db = np.sum((Y - Y_pre)*(-1))/len(Y)
        return dw, db

    def change_params(self, dw, db, alpha):
        self.m_W = self.m_W - alpha*dw
        self.m_b = self.m_b - alpha*db

    def train(self, X, Y, alpha, epochs, _lambda, reg):
        error_list = []
        time_stamp = []
        for i in range(epochs):
            if reg == "L2":
                loss, y_pred = self.Loss_L2(X, Y, _lambda)
            else:
                loss, y_pred = self.Loss_L1(X, Y, _lambda)
            time_stamp.append(i)
            error_list.append(loss)
            dw, db = self.dL(X, Y, y_pred)
            self.change_params(dw, db, alpha)
            # print("error de pérdida : " + str(loss))
            if (i % 100 == 0):
                print("error de pérdida : " + str(loss))
                #    self.plot_error(time_stamp, error_list)
                # self.plot_plane(X[:, 0], X[:, 1], Y)

        return time_stamp, error_list

    def plot_error(self, time, loss):
        ptl.plot(time, loss)
        ptl.show()

    def plot_line(self, x, y_pre):
        ptl.plot(x, y_pre, '*')
        ptl.plot(x, self.H(x))
        ptl.show()

    def plot_plane(self, xx, yy, zz):
        fig = ptl.figure(figsize=(8, 8))

        # crear un subplot 3D
        ax = fig.add_subplot(111, projection='3d')
        ax.scatter(xx, yy, zz, c='r', marker='o')

        # definir el tamaño del plano
        x = np.linspace(min(xx), max(xx), len(xx))
        y = np.linspace(min(yy), max(yy), len(yy))

        x, y = np.meshgrid(x, y)

        # definir la ecuación del plano
        z = self.m_W[0]*x + self.m_W[1]*y + self.m_b

        # dibujar el plano
        ax.plot_surface(x, y, z)
        ax.plot_surface

        ax.view_init(elev=0, azim=70)

        # agregar etiquetas a los ejes
        ax.set_xlabel('Feacture 1: X0')
        ax.set_ylabel('Feacture 2: X1')
        ax.set_zlabel('Prediction')

        ax.mouse_init()

        ptl.show()


variables_list = [
    "PROGRAMA_PPTAL",
    "TIPO_PROD_PROY",
    "PRODUCTO_PROYECTO",
    "TIPO_ACT_OBRA_ACCINV",
    "ACT_OBRA_ACCINV",
    "DIVISION_FN",
    "GRUPO_FN",
    "FINALIDAD",
    "UNIDAD_MEDIDA",
    "DEPARTAMENTO",
    "PROVINCIA",
    "DISTRITO",
    "UBIGEO",
    "FUENTE_FINANC",
    "SUBGENERICA",
    "SUBGENERICA_DET",
    "ESPECIFICA",
    "ESPECIFICA_DET"
]

df = pd.read_csv('./PROYECTO_1/train.csv')
df_test = pd.read_csv('./PROYECTO_1/test.csv')


y_train = df['MTO_PIA'].to_numpy()

df_train = df[variables_list]
df_test = df_test[variables_list]


encoder = ce.TargetEncoder(cols=variables_list, return_df=True)
encoder_test = ce.TargetEncoder(cols=variables_list, return_df=True)

df_train_encoded = encoder.fit_transform(df_train, y_train)
df_test_encoded = encoder_test.fit_transform(df_test, y_train)

# ONLY PUT THE COLUMNS PROGRAMA_PPTAL, ACT_OBRA_ACCINV Y FUENTE_FINANC
x_train = df_train_encoded[['PROGRAMA_PPTAL',
                            'ACT_OBRA_ACCINV', 'FUENTE_FINANC']].to_numpy()
x_test = df_test_encoded[['PROGRAMA_PPTAL',
                          'ACT_OBRA_ACCINV', 'FUENTE_FINANC']].to_numpy()

print(x_train)
print(x_test)


feactures = 3

y_min = np.min(y_train)
y_max = np.max(y_train)

x_train = (x_train - np.min(x_train)) / (np.max(x_train) - np.min(x_train))
y_train = (y_train - np.min(y_train)) / (np.max(y_train) - np.min(y_train))

# # rst = input("Quiere ver los resultados : (Y/Ny)")
LR = LinearRegresion(3)
time, loss = LR.train(x_train, y_train, 0.5, 10000, 0.0000000001, "L1")
LR.plot_error(time, loss)
# # rst = input("Quiere la aproxmación de la función : (Y/Ny)")
# rst = input("Vericar el error de salida : (Y/N)")
# LR.plot_line(x_train, LR.H(x_train))
# LR.plot_plane(x[:,0],x[:,1],y)
