import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns


class NonLinearRegresion:
    def __init__(self, grado):
        np.random.seed(2005)
        self.m_W = np.random.rand(grado+1)
        self.m_b = np.random.random()
        self.grado = grado

    def H(self, X):
        return np.dot(X, self.m_W)

    def predic(self, x):
        potencias = np.arange(self.grado + 1)
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
        # db = np.sum((Y - Y_pre)*(-1))/len(Y)
        return dw

    def change_params(self, dw, alpha):
        self.m_W = self.m_W - alpha*dw
        # self.m_b = self.m_b - alpha*db

    def train(self, X, Y, alpha, epochs, lambda_, reg):
        error_list = []
        time_stamp = []

        potencias = np.arange(self.grado + 1)
        X = np.power.outer(X, potencias)

        for i in range(epochs):
            if reg == "L2":
                loss, y_pred = self.Loss_L2(X, Y, lambda_)
            elif reg == "L1":
                loss, y_pred = self.Loss_L1(X, Y, lambda_)
            time_stamp.append(i)
            error_list.append(loss)
            dw = self.dL(X, Y, y_pred, lambda_)
            self.change_params(dw, alpha)

            if (i % 1000 == 0):
                # self.plot_error(time_stamp, error_list)
                print("error de pérdida : " + str(loss))
                # LR.plot_line(x, LR.predic(x))
        return time_stamp, error_list

    def plot_error(self, time, loss, reg_type):
        color = 'red' if reg_type == 'L1' else 'blue'
        sns.lineplot(x=time, y=loss, color=color)
        plt.title(f'Error de entrenamiento - Grado {self.grado} - {reg_type}')
        plt.xlabel('Epochs')
        plt.savefig(f"Error_Grado_{self.grado}_{reg_type}.png")
        plt.close()

    def plot_line(self, x, y_pre,  reg_type, etiquetax='META', etiquetay='MTO_PIA'):
        color = 'red' if reg_type == 'L1' else 'blue'
        sns.lineplot(x=x, y=y_pre, label=f'Grado {
                     self.grado} - {reg_type}', color=color)
        plt.xlabel(etiquetax)
        plt.ylabel(etiquetay)
        plt.title(f'Línea de Regresión - Grado {self.grado} - {reg_type}')
        plt.legend()
        plt.savefig(f"Prediccion_Grado_{self.grado}_{reg_type}.png")
        plt.close()


# Cargamos el dataset
df = pd.read_csv('./PROYECTO_1/train.csv')
df_test = pd.read_csv('./PROYECTO_1/test.csv')

# # # Obtenemos los datos de y
y_train = df['MTO_PIA'].to_numpy()

# # Total de variables hasta antes de MTO_PIA
# pos_mto_pia = df.columns.get_loc('MTO_PIA')
# print(pos_mto_pia)

# # Total de registros
# print(df.shape)

# # obtener todos los header con datos unicamente numericos antes de MTO_PIA
# posible_x_headers = ['FECHA_CORTE', 'ANIO_EJEC', 'SEC_EJEC', 'META', 'CANT_META_ANUAL',
#                      'CANT_META_SEM', 'AVAN_FISICO_ANUAL', 'AVAN_FISICO_SEM', 'SEC_FUNC']


# # # OBTENER LA CORRELACION DE LOS POSIBLES CANDIDATOS DE X RESPECTO A Y PARA SABER CUAL O CUALES SON LOS MEJORES CANDIDATOS
# # for header in posible_x_headers:
# #     x_train = df[header]
# #     x_train = x_train.to_numpy()
# #     corr = np.corrcoef(x_train, y_train)
# #     print('Correlación entre', header, 'y MTO_PIA:', corr[0, 1]**2)

# # # GRAFICAR LA CORRELACION DE LOS POSIBLES CANDIDATOS DE X RESPECTO A Y
# # for header in posible_x_headers:
# #     x_train = df[header]
# #     x_train = x_train.to_numpy()
# #     plt.title(header + ' vs MTO_PIA')
# #     plt.scatter(x_train, y_train)
# #     plt.xlabel(header)
# #     plt.ylabel('MTO_PIA')
# #     plt.show()

# # x_train_1 = df['META'].to_numpy()
# # x_train_2 = df['CANT_META_SEM'].to_numpy()
# # x_train_3 = df['AVAN_FISICO_SEM'].to_numpy()

# # plot original graph

x_min = np.min(df['META'])
x_max = np.max(df['META'])
y_min = np.min(y_train)
y_max = np.max(y_train)

x_train = df['META'].to_numpy()
x_test = df_test['META'].to_numpy()

# Normalizar los datos
x_train = (x_train - np.min(x_train)) / (np.max(x_train) - np.min(x_train))
y_train = (y_train - np.min(y_train)) / (np.max(y_train) - np.min(y_train))
x_testl1 = (x_test - np.min(x_test)) / (np.max(x_test) - np.min(x_test))
x_testl2 = (x_test - np.min(x_test)) / (np.max(x_test) - np.min(x_test))

grado = 5

NLRL1 = NonLinearRegresion(grado)
timeL1, errorL1 = NLRL1.train(x_train, y_train, 0.99, 100000, 0.0000001, "L1")
y_predL1 = NLRL1.predic(x_testl1)
y_predL1 = y_predL1 * (y_max - y_min) + y_min
x_testl1 = x_testl1 * (x_max - x_min) + x_min


NLRL2 = NonLinearRegresion(grado)
timeL2, errorL2 = NLRL2.train(x_train, y_train, 0.99, 100000, 0.0000001, "L2")
y_predL2 = NLRL2.predic(x_testl2)
y_predL2 = y_predL2 * (y_max - y_min) + y_min
x_testl2 = x_testl2 * (x_max - x_min) + x_min

# # # GRAFICAR EL ERROR L1 Y L2 EN UN SOLO GRAFICO CON SEA BORN
sns.lineplot(x=timeL1, y=errorL1, label='L1')
sns.lineplot(x=timeL2, y=errorL2, label='L2')
plt.title('Error de entrenamiento - Grado {}'.format(grado))
plt.xlabel('Epochs')
plt.ylabel('Error')
plt.legend()
plt.savefig("Error_Grado_{}.png".format(grado))
plt.close()

# # # GRAFICAR LA LINEA DE REGRESION L1 Y L2 EN UN SOLO GRAFICO CON SEA BORN
sns.lineplot(x=x_testl1, y=y_predL1, label='Grado 2 - L1')
sns.lineplot(x=x_testl2, y=y_predL2, label='Grado 2 - L2')
plt.xlabel('META')
plt.ylabel('MTO_PIA')
plt.title('Línea de Regresión - Grado {}'.format(grado))
plt.legend()
plt.savefig("Prediccion_Grado_{}.png".format(grado))
plt.close()


# # WRITE THE DATA IN A NEW CSV

id = list(range(1, len(y_predL2) + 1))
newdf = pd.DataFrame({
    'ID': id,
    'MTO_PIA': y_predL2
})
newdf.to_csv('submission.csv', index=False)
