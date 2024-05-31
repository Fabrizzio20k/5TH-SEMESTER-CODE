import os
import re
import nltk
import heapq
import pandas as pd
from nltk.corpus import stopwords
from nltk.tokenize import word_tokenize
from nltk.stem.snowball import SnowballStemmer
from collections import defaultdict, Counter
from math import log, sqrt

# Descargar los recursos necesarios de NLTK
nltk.download('punkt')
nltk.download('stopwords')

# Inicializar el stemmer y las stopwords
stemmer = SnowballStemmer("spanish")
stop_words = set(stopwords.words("spanish"))


def tokenize(text):
    """ Tokeniza, elimina stopwords y aplica stemming a las palabras """
    tokens = word_tokenize(text.lower())
    filtered_tokens = [stemmer.stem(
        token) for token in tokens if token.isalpha() and token not in stop_words]
    return filtered_tokens


def compute_tf(tokens):
    """ Calcula la frecuencia de término (TF) """
    tf = Counter(tokens)
    total_terms = len(tokens)
    for term in tf:
        tf[term] /= total_terms
    return tf


def compute_idf(documents):
    """ Calcula la frecuencia inversa de documentos (IDF) """
    N = len(documents)
    df = defaultdict(int)
    for doc in documents:
        unique_terms = set(doc)
        for term in unique_terms:
            df[term] += 1
    idf = {term: log(N / df[term]) for term in df}
    return idf


def compute_norm(tf_idf):
    """ Calcula la norma de un documento """
    return sqrt(sum(weight**2 for weight in tf_idf.values()))


def spimi_invert(documents, block_size):
    """ Construye el índice invertido usando SPIMI """
    block_id = 0
    term_dict = defaultdict(lambda: defaultdict(float))
    doc_norms = {}
    idf = compute_idf(documents)

    for doc_id, tokens in enumerate(documents):
        tf = compute_tf(tokens)
        tf_idf = {term: tf[term] * idf[term] for term in tf}
        doc_norms[doc_id] = compute_norm(tf_idf)

        for term, weight in tf_idf.items():
            term_dict[term][doc_id] = weight

        # Si el bloque excede el tamaño permitido, escribir a disco y limpiar
        if len(term_dict) >= block_size:
            write_block_to_disk(term_dict, block_id)
            block_id += 1
            term_dict.clear()

    if term_dict:  # Escribir el último bloque si queda algo
        write_block_to_disk(term_dict, block_id)
        block_id += 1

    return doc_norms, block_id


def write_block_to_disk(term_dict, block_id):
    """ Escribe un bloque de índice invertido a disco """
    filename = f'block_{block_id}.txt'
    with open(filename, 'w') as f:
        for term, postings in sorted(term_dict.items()):
            postings_list = ' '.join(
                f"{doc_id}:{weight}" for doc_id, weight in postings.items())
            f.write(f"{term}: {postings_list}\n")
    print(f"Escrito {filename}")


def merge_blocks(block_count, output_file):
    """ Fusiona bloques de índices invertidos en un solo archivo """
    heap = []
    file_pointers = [open(f'block_{i}.txt', 'r') for i in range(
        block_count) if os.path.exists(f'block_{i}.txt')]
    for i, fp in enumerate(file_pointers):
        term, postings = read_next_line(fp)
        if term:
            heapq.heappush(heap, (term, postings, i))

    with open(output_file, 'w') as f:
        while heap:
            term, postings, i = heapq.heappop(heap)
            f.write(f"{term}: {postings}\n")
            next_line = file_pointers[i].readline().strip()
            if next_line:
                next_term, next_postings = next_line.split(': ', 1)
                heapq.heappush(heap, (next_term, next_postings, i))

    for fp in file_pointers:
        fp.close()
    print(f"Índice invertido final escrito en {output_file}")


def read_next_line(file_pointer):
    """ Lee la siguiente línea de un archivo y retorna el término y sus postings """
    line = file_pointer.readline().strip()
    if line:
        term, postings = line.split(': ', 1)
        return term, postings
    return None, None


def cosine_similarity(query_vector, doc_vector, doc_norm):
    """ Calcula la similitud de coseno entre dos vectores """
    dot_product = sum(
        query_vector[term] * doc_vector.get(term, 0) for term in query_vector)
    return dot_product / doc_norm if doc_norm else 0.0


def process_query(query, idf, doc_norms, inverted_index, top_k=5):
    """ Procesa una consulta y retorna los top-k documentos relevantes """
    tokens = tokenize(query)
    tf_query = compute_tf(tokens)
    tf_idf_query = {term: tf_query[term] *
                    idf.get(term, 0) for term in tf_query}
    query_norm = compute_norm(tf_idf_query)

    scores = defaultdict(float)

    for term, query_weight in tf_idf_query.items():
        if term in inverted_index:
            for doc_id, doc_weight in inverted_index[term].items():
                scores[doc_id] += query_weight * doc_weight

    ranked_scores = sorted(((score / (doc_norms[doc_id] * query_norm), doc_id)
                            for doc_id, score in scores.items()), reverse=True)

    return ranked_scores[:top_k]


# Ejemplo de uso
# Leer dataset
# Supongamos que tenemos un dataset en formato de tabla con columnas 'id' y 'text'
data = pd.DataFrame({
    'id': [1, 2, 3, 4],
    'text': [
        "Pedrito compró un abanico",
        "Juanito compró un ventilador",
        "Maria db es una base de datos",
        "Maria tiene una base de datos llamada db que es muy popular y aparece en muchos trabajos"
    ]
})

# Preprocesar los documentos
documents = [tokenize(text) for text in data['text']]
block_size = 10  # Ajusta el tamaño del bloque según la memoria disponible

# Construcción del índice invertido
doc_norms, total_blocks = spimi_invert(documents, block_size)
merge_blocks(total_blocks, 'final_inverted_index.txt')

# Crear un índice invertido en memoria secundaria para la consulta
inverted_index = defaultdict(dict)
with open('final_inverted_index.txt', 'r') as f:
    for line in f:
        term, postings = line.split(': ', 1)
        for posting in postings.split():
            doc_id, weight = posting.split(':')
            inverted_index[term][int(doc_id)] = float(weight)

# Proceso de consulta
query = "Tengo una base de datos muy popular llamada db"
idf = compute_idf(documents)
top_k_results = process_query(query, idf, doc_norms, inverted_index)

print("Top-k documentos relevantes:")
for score, doc_id in top_k_results:
    print(f"Documento {doc_id} con score {score}")
