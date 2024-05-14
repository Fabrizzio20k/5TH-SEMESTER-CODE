import pandas as pd
import numpy as np
import nltk         # Librería de Natural Language

# nltk.download('punkt')

# Clase para leer los archivos


class BoolModel:
    def __init__(self, files):
        self.files = files
        self.words = set()
        self.matrix = None
        self.dataframe = None

        stoplist = []
        with open('stoplist.txt', 'r', encoding='utf-8') as f:
            for line in f:
                stoplist.append(line.strip())
        stoplist += ['?', '-', ':', ',', '.', '!', '¡', '¿', ';',
                     '(', ')', '«', '»', '—', '“', '”', '…', '°', 'º', '``']
        self.stoplist = stoplist

    def __convert_to_lowercase(self):
        for file in self.files:
            with open(file, 'r', encoding='utf-8') as f:
                text = f.read()
            text = text.lower()
            with open(file, 'w', encoding='utf-8') as f:
                f.write(text)

    def __remove_stopwords(self):
        for file in self.files:
            with open(file, 'r', encoding='utf-8') as f:
                text = f.read()
            words = nltk.word_tokenize(text)
            words = [word for word in words if word not in self.stoplist]
            text = ' '.join(words)

            new_name = file.split('.')[0] + '_clean.txt'
            with open(new_name, 'w', encoding='utf-8') as f:
                f.write(text)

            files[files.index(file)] = new_name

    def __create_set_words(self):
        for file in self.files:
            with open(
                    file, 'r', encoding='utf-8') as f:
                text = f.read()
            words = nltk.word_tokenize(text)
            self.words.update(words)

    def preprocesar(self):
        self.__convert_to_lowercase()
        self.__remove_stopwords()

    def create_incidence_matrix(self):
        self.__create_set_words()
        self.matrix = np.zeros((len(self.words), len(self.files))).astype(int)

        for i, file in enumerate(self.files):
            with open(file, 'r', encoding='utf-8') as f:
                text = f.read()
            words = nltk.word_tokenize(text)
            for word in words:
                if word in self.words:
                    self.matrix[list(self.words).index(word)][i] = 1

        self.dataframe = pd.DataFrame(self.matrix, index=list(
            self.words), columns=self.files)
        self.dataframe = self.dataframe.sort_index()

        self.dataframe.to_csv('incidence_matrix.csv')

    def print_pretty(self):
        print(self.dataframe)

    def M(self, palabra):
        message = ''
        try:
            message = self.matrix[list(self.words).index(palabra)]
        except Exception as e:
            message = ['No existe la palabra', e]
        finally:
            return message

    def AND(self, word1, word2):
        if len(word1) != len(word2):
            return 'No se pueden comparar'
        res = np.logical_and(word1, word2)
        return res

    def OR(self, word1, word2):
        if len(word1) != len(word2):
            return 'No se pueden comparar'
        res = np.logical_or(word1, word2)
        return res

    def NOT(self, word1):
        res = np.logical_not(word1)
        return res

    def query(self, expresion):
        res = []
        for i in range(len(expresion)):
            if expresion[i]:
                res.append(self.files[i])
        return res


files = ['libro1.txt', 'libro2.txt', 'libro3.txt',
         'libro4.txt', 'libro5.txt', 'libro6.txt']

f = BoolModel(files)
f.preprocesar()

f.create_incidence_matrix()
f.print_pretty()

# Consulta 1: éomer AND éored OR abierto AND acaba
a = f.AND(f.M('éomer'), f.M('éored'))
b = f.AND(f.M('abierto'), f.M('acaba'))
c = f.OR(a, b)

print(f.query(c))

# Consulta 2: NOT anillo AND NOT hobbit
a = f.NOT(f.M('anillo'))
b = f.NOT(f.M('hobbit'))
c = f.AND(a, b)

print(f.query(c))

# Consulta 3: éowyn AND aparecer AND apoderarse OR NOT aragorn
a = f.AND(f.M('éowyn'), f.M('aparecer'))
b = f.AND(a, f.M('apoderarse'))
c = f.OR(b, f.NOT(f.M('aragorn')))
print(f.query(c))
