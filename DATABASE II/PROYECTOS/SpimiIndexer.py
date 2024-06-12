import os
import pandas as pd
import nltk
from nltk.corpus import stopwords
from nltk.tokenize import word_tokenize
from nltk.stem import PorterStemmer, SnowballStemmer
from collections import defaultdict, Counter
import numpy as np
import pickle
import heapq
import time
from langdetect import detect

nltk.download('punkt')
nltk.download('stopwords')


class SPIMIIndexer:
    def __init__(self, csv_path, block_size=5000, temp_dir='temp_blocks', final_index_file='final_index.pkl'):
        # Leer el archivo CSV
        self.df = pd.read_csv(csv_path)
        # Obtener las letras de las canciones
        self.lyrics = self.df['lyrics'].fillna('').tolist()
        # Obtener metadatos de las canciones
        self.song_metadata = self.df[[
            'track_name', 'track_artist', 'track_album_name']].to_dict('records')
        self.block_size = block_size
        self.temp_dir = temp_dir
        self.final_index_file = final_index_file
        self.num_docs = len(self.lyrics)
        self.stop_words = set(stopwords.words('spanish')).union(
            set(stopwords.words('english')))

        # Crear el directorio temporal si no existe
        if not os.path.exists(self.temp_dir):
            os.makedirs(self.temp_dir)

        # Crear el índice invertido en bloques y luego fusionar los bloques
        self.spimi_invert()
        self.__merge_blocks()
        self.__cleanup_temp_files()

    def preprocess(self, text):
        # Tokenización, eliminación de stopwords y stemming considerando el idioma. a veces el text es vacio ""
        if text == "" or text == None:
            return []
        else:
            if detect(text) == 'es':
                stemmer = SnowballStemmer('spanish')
            else:
                stemmer = PorterStemmer()

        tokens = word_tokenize(text.lower())
        filtered_tokens = [stemmer.stem(
            word) for word in tokens if word.isalnum() and word not in self.stop_words]
        return filtered_tokens

    def spimi_invert(self):
        block_id = 0
        # Procesar los documentos en bloques
        for i in range(0, len(self.lyrics), self.block_size):
            block = self.lyrics[i:i + self.block_size]
            dictionary = defaultdict(list)
            doc_norms = defaultdict(float)

            for doc_id, text in enumerate(block):
                tokens = self.preprocess(text)
                term_freq = Counter(tokens)

                for term, freq in term_freq.items():
                    dictionary[term].append((i + doc_id, freq))
                    doc_norms[i + doc_id] += (freq ** 2)

            for doc_id in doc_norms:
                doc_norms[doc_id] = np.sqrt(doc_norms[doc_id])

            # Guardar el bloque en un archivo temporal
            block_path = os.path.join(self.temp_dir, f'block_{block_id}.pkl')
            with open(block_path, 'wb') as f:
                pickle.dump((dictionary, doc_norms), f)

            block_id += 1

    def __merge_blocks(self):
        block_files = [os.path.join(self.temp_dir, f)
                       for f in os.listdir(self.temp_dir)]
        heap = []
        term_postings = defaultdict(list)
        doc_norms = defaultdict(float)

        for block_file in block_files:
            with open(block_file, 'rb') as f:
                dictionary, block_doc_norms = pickle.load(f)
                for term, postings in dictionary.items():
                    for posting in postings:
                        heapq.heappush(heap, (term, posting))
                for doc_id, norm in block_doc_norms.items():
                    if doc_id in doc_norms:
                        doc_norms[doc_id] += norm ** 2
                    else:
                        doc_norms[doc_id] = norm ** 2

        while heap:
            term, posting = heapq.heappop(heap)
            term_postings[term].append(posting)

        for doc_id in doc_norms:
            doc_norms[doc_id] = np.sqrt(doc_norms[doc_id])

        # Guardar el índice final en un archivo
        with open(self.final_index_file, 'wb') as f:
            pickle.dump((term_postings, doc_norms), f)

        self.dictionary = term_postings
        self.doc_norms = doc_norms

    def __cleanup_temp_files(self):
        # Eliminar los archivos temporales
        for block_file in os.listdir(self.temp_dir):
            os.remove(os.path.join(self.temp_dir, block_file))
        os.rmdir(self.temp_dir)

    def load_final_index(self):
        # Cargar el índice final desde el archivo
        with open(self.final_index_file, 'rb') as f:
            self.dictionary, self.doc_norms = pickle.load(f)

    def compute_tfidf(self, term, doc_id):
        # Calcular el peso TF-IDF para un término en un documento
        term_postings = self.dictionary.get(term, [])
        doc_freq = len(term_postings)
        tf = next((freq for doc, freq in term_postings if doc == doc_id), 0)
        idf = np.log(self.num_docs / (1 + doc_freq))
        return tf * idf

    def cosine_similarity(self, query):
        # Preprocesar la consulta
        query_tokens = self.preprocess(query)
        query_vector = Counter(query_tokens)

        # Normalizar el vector de la consulta
        query_tfidf_vector = {}
        query_norm = 0
        for term, count in query_vector.items():
            if term in self.dictionary:
                idf = np.log(self.num_docs / (1 + len(self.dictionary[term])))
                query_tfidf = count * idf
                query_tfidf_vector[term] = query_tfidf
                query_norm += query_tfidf ** 2

        query_norm = np.sqrt(query_norm)
        if query_norm == 0:
            query_norm = 1  # para evitar la división por cero

        scores = defaultdict(float)

        # Calcular la similitud de coseno entre la consulta y los documentos
        for term, query_tfidf in query_tfidf_vector.items():
            query_tfidf /= query_norm
            if term in self.dictionary:
                for doc_id, freq in self.dictionary[term]:
                    doc_tfidf = self.compute_tfidf(
                        term, doc_id) / self.doc_norms[doc_id]
                    scores[doc_id] += query_tfidf * doc_tfidf

        return scores

    def retrieve_top_k(self, query, k=5, additional_features=None):
        if additional_features is None:
            additional_features = []

        self.load_final_index()  # Cargar el índice final para consultas

        start_time = time.time()
        scores = self.cosine_similarity(query)
        sorted_scores = sorted(
            scores.items(), key=lambda item: item[1], reverse=True)
        top_k_results = sorted_scores[:k]

        results = []
        for doc_id, score in top_k_results:
            metadata = self.song_metadata[doc_id]
            result = {
                'track_name': metadata['track_name'],
                'row_position': doc_id,
                'cosine_similarity': score
            }
            for feature in additional_features:
                if feature in self.df.columns:
                    result[feature] = self.df.iloc[doc_id][feature]
            results.append(result)

        end_time = time.time()

        # Retornar un diccionario con el tiempo total de la consulta y los resultados
        return {
            'query_time': end_time - start_time,
            'results': results
        }
