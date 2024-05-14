import pandas as pd
import numpy as np
import nltk         # Librería de Natural Language

# nltk.download('punkt')

# Clase para leer los archivos


class Files:
    def __init__(self, files):
        self.files = files
        self.words = set()
        self.matrix = None

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

    def __create_set_words(self):
        for file in self.files:
            with open(file, 'r', encoding='utf-8') as f:
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

    def print_pretty(self):
        df = pd.DataFrame(self.matrix, index=list(
            self.words), columns=self.files)
        # ordenar en orden alfabetico
        df = df.sort_index()
        print(df)

    def M(self, palabra):
        message = ''
        try:
            message = self.matrix[list(self.words).index(palabra)]
        except Exception as e:
            message = ['No existe la palabra', e]
        finally:
            return message

    def AND(self, word1, word2):
        # return string with the files that contain both words. previous to that, word1 and word2 are
        # binary arrays with the files that contain the words.
        res = np.logical_and(self.M(word1), self.M(word2))
        # return as the same as M
        return res

    def OR(self, word1, word2):
        # return string with the files that contain both words. previous to that, word1 and word2 are
        # binary arrays with the files that contain the words.
        res = np.logical_or(self.M(word1), self.M(word2))
        # return as the same as M
        return res

    def NOT(self, word1):
        # return string with the files that contain both words. previous to that, word1 and word2 are
        # binary arrays with the files that contain the words.
        res = np.logical_not(self.M(word1))
        # return as the same as M
        return res


files = ['libro1.txt', 'libro2.txt', 'libro3.txt',
         'libro4.txt', 'libro5.txt', 'libro6.txt']

fileHandler = Files(files)
fileHandler.preprocesar()

fileHandler.create_incidence_matrix()
fileHandler.print_pretty()

print(fileHandler.AND('abierto', 'acaba'))
