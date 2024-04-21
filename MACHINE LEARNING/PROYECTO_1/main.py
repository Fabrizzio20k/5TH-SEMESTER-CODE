import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns


class LinearRegresion:
    def __init__(self, grado):
        self.m_W = np.random.rand(grado)
        self.m_b = np.random.random()
        self.grado = grado

    def H(self, X):
        return np.dot(X, self.m_W)

    def predic(self, x):
        potencias = np.arange(self.grado)
        x = np.power.outer(x, potencias)
        return np.dot(x, self.m_W)

    def Loss_L2(self, X, Y, lambda_):
        y_pred = self.H(X)
        return (np.linalg.norm((Y - y_pred))**2)/(2*len(Y)), y_pred + lambda_*np.linalg.norm(self.m_W, 2)

    def Loss_L1(self, X, Y, lambda_):
        y_pred = self.H(X)
        return (np.linalg.norm((Y - y_pred))**2) / (2 * len(Y)), y_pred + lambda_ * np.linalg.norm(self.m_W, 1)

    def dL(self, X, Y, Y_pre, lambda_):
        dw = np.matmul(Y - Y_pre, -X)/len(Y) + 2*lambda_*self.m_W
        db = np.sum((Y - Y_pre)*(-1))/len(Y)
        return dw, db

    def change_params(self, dw, db, alpha):
        self.m_W = self.m_W - alpha*dw
        # self.m_b = self.m_b - alpha*db

    def train(self, X, Y, alpha, epochs, lambda_, reg):
        error_list = []
        time_stamp = []

        potencias = np.arange(self.grado)
        X = np.power.outer(X, potencias)

        for i in range(epochs):
            if reg == "L2":
                loss, y_pred = self.Loss_L2(X, Y, lambda_)
            else:
                loss, y_pred = self.Loss_L1(X, Y, lambda_)
            time_stamp.append(i)
            error_list.append(loss)
            dw, db = self.dL(X, Y, y_pred, lambda_)
            self.change_params(dw, db, alpha)

            if (i % 1000 == 0):
                # self.plot_error(time_stamp, error_list)
                print("error de pérdida : " + str(loss))
                # LR.plot_line(x, LR.predic(x))
        return time_stamp, error_list

    def plot_error(self, time, loss):
        # GRAFICAR EL ERROR CON SEABORN
        sns.lineplot(x=time, y=loss)
        plt.show()

    def plot_line(self, x, y_pre):
        # GRAFICAR LA LINEA DE REGRESION CON SEABORN
        sns.scatterplot(x=x, y=y_pre)
        plt.show()

# posible_x_headers = ['FECHA_CORTE', 'ANIO_EJEC', 'SEC_EJEC', 'META', 'CANT_META_ANUAL', 'CANT_META_SEM', 'AVAN_FISICO_ANUAL', 'AVAN_FISICO_SEM', 'SEC_FUNC']
# obtener todos los header con datos unicamente numericos antes de MTO_PIA

# #OBTENER LA CORRELACION DE LOS POSIBLES CANDIDATOS DE X RESPECTO A Y PARA SABER CUAL O CUALES SON LOS MEJORES CANDIDATOS
# for header in posible_x_headers:
#     x_train = df[header]
#     x_train = x_train.to_numpy()
#     corr = np.corrcoef(x_train, y_train)
#     print('Correlación entre', header, 'y MTO_PIA:', corr[0, 1])

# GRAFICAR LA CORRELACION DE LOS POSIBLES CANDIDATOS DE X RESPECTO A Y
# for header in posible_x_headers:
#     x_train = df[header]
#     x_train = x_train.to_numpy()
#     plt.scatter(x_train, y_train)
#     plt.xlabel(header)
#     plt.ylabel('MTO_PIA')
#     plt.show()

# x_train_1 = df['META'].to_numpy()
# x_train_2 = df['CANT_META_SEM'].to_numpy()
# x_train_3 = df['AVAN_FISICO_SEM'].to_numpy()


# Cargamos el dataset
df = pd.read_csv('./PROYECTO_1/train.csv')
df_test = pd.read_csv('./PROYECTO_1/test.csv')

x_train = df['SEC_FUNC'].to_numpy()
x_test = df_test['SEC_FUNC'].to_numpy()
y_train = df['MTO_PIA'].to_numpy()

# Normalizar los datos
x_train = (x_train - np.min(x_train)) / (np.max(x_train) - np.min(x_train))
y_train = (y_train - np.min(y_train)) / (np.max(y_train) - np.min(y_train))
x_test = (x_test - np.min(x_test)) / (np.max(x_test) - np.min(x_test))

LR = LinearRegresion(3)
time, error = LR.train(x_train, y_train, 0.1, 10000, 0.00001, "L2")
LR.plot_error(time, error)
# Desnormalizar los datos
y_pred = LR.predic(x_test)
y_pred = y_pred * (np.max(y_train) - np.min(y_train)) + np.min(y_train)
x_test = x_test * (np.max(x_train) - np.min(x_train)) + np.min(x_train)
LR.plot_line(x_test, y_pred)
