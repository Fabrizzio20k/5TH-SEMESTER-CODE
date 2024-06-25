select * from film;

/* --------------------- INDEXING ---------------------------------- */
ALTER TABLE film ADD COLUMN weighted_tsv tsvector;
ALTER TABLE film ADD COLUMN weighted_tsv2 tsvector;
-- Storing and Populating Document Vectors
UPDATE film SET
weighted_tsv = x.weighted_tsv,
weighted_tsv2 = x.weighted_tsv
FROM (
SELECT film_id,
setweight(to_tsvector('english', COALESCE(title,'')), 'A') ||
setweight(to_tsvector('english', COALESCE(description,'')), 'B')
AS weighted_tsv
FROM film
) AS x
WHERE x.film_id = film.film_id;
-- Indexing
/**
* GIN: Generalized Inverted Index: default index type for full text searching
* GiST: Generalized Search Tree: traditional database-style index
*/
CREATE INDEX weighted_tsv_idx ON film USING GIN (weighted_tsv2);

/* ----------------------------Querying------------------------------------- */
-- Parser Tsquery
SELECT plainto_tsquery('english', 'The Man or the Woman');
SELECT to_tsquery('english', 'Man | Woman');
-- Query normal
explain analyze
select title, description
from (select *, (title || description) as texto from film ) T
where T.texto ilike '%man%' OR T.texto ilike '%woman%';
-- Query textual
explain analyze
SELECT title, description
FROM film
WHERE to_tsquery('english', 'Man | Woman') @@ weighted_tsv;
-- Ranking (Top k) sobre el atributo no indexado
/* ts_rank or ts_rank_cd*/
set enable_seqscan = false;
explain analyze
SELECT title, description, ts_rank_cd(weighted_tsv, query) AS rank
FROM film, to_tsquery('english', 'Man | Woman') query
WHERE query @@ weighted_tsv
ORDER BY rank DESC
LIMIT 10;
-- Ranking (Top k) sobre el atributo indexado
/* ts_rank or ts_rank_cd*/
set enable_seqscan = false;
explain analyze
SELECT title, description, ts_rank_cd(weighted_tsv2, query) AS rank
FROM film, to_tsquery('english', 'Man | Woman') query
WHERE query @@ weighted_tsv
ORDER BY rank DESC
LIMIT 10;






-- Agregar columnas para tsvector
ALTER TABLE film ADD COLUMN weighted_tsv tsvector;
ALTER TABLE film ADD COLUMN weighted_tsv2 tsvector;

-- Almacenar y poblar los vectores de documentos
UPDATE film SET
weighted_tsv = x.weighted_tsv,
weighted_tsv2 = x.weighted_tsv
FROM (
    SELECT film_id,
           setweight(to_tsvector('english', COALESCE(title, '')), 'A') ||
           setweight(to_tsvector('english', COALESCE(description, '')), 'B') AS weighted_tsv
    FROM film
) AS x
WHERE x.film_id = film.film_id;

-- Crear índice GIN en weighted_tsv2
CREATE INDEX weighted_tsv_idx ON film USING GIN (weighted_tsv2);



-- Consultas para diferentes cantidades de elementos sin índice

-- 250 elementos
EXPLAIN ANALYZE
SELECT title, description
FROM (
    SELECT *, (title || description) AS texto
    FROM film
    LIMIT 250
) T
WHERE T.texto ILIKE '%man%' OR T.texto ILIKE '%woman%';

-- 500 elementos
EXPLAIN ANALYZE
SELECT title, description
FROM (
    SELECT *, (title || description) AS texto
    FROM film
    LIMIT 500
) T
WHERE T.texto ILIKE '%man%' OR T.texto ILIKE '%woman%';

-- 750 elementos
EXPLAIN ANALYZE
SELECT title, description
FROM (
    SELECT *, (title || description) AS texto
    FROM film
    LIMIT 750
) T
WHERE T.texto ILIKE '%man%' OR T.texto ILIKE '%woman%';

-- 1000 elementos
EXPLAIN ANALYZE
SELECT title, description
FROM (
    SELECT *, (title || description) AS texto
    FROM film
    LIMIT 1000
) T
WHERE T.texto ILIKE '%man%' OR T.texto ILIKE '%woman%';

-- Consultas para diferentes cantidades de elementos con índice

-- 250 elementos
SET enable_seqscan = false;
EXPLAIN ANALYZE
SELECT title, description, ts_rank_cd(weighted_tsv, query) AS rank
FROM film, to_tsquery('english', 'Man | Woman') query
WHERE query @@ weighted_tsv
ORDER BY rank DESC
LIMIT 250;

-- 500 elementos
SET enable_seqscan = false;
EXPLAIN ANALYZE
SELECT title, description, ts_rank_cd(weighted_tsv, query) AS rank
FROM film, to_tsquery('english', 'Man | Woman') query
WHERE query @@ weighted_tsv
ORDER BY rank DESC
LIMIT 500;

-- 750 elementos
SET enable_seqscan = false;
EXPLAIN ANALYZE
SELECT title, description, ts_rank_cd(weighted_tsv, query) AS rank
FROM film, to_tsquery('english', 'Man | Woman') query
WHERE query @@ weighted_tsv
ORDER BY rank DESC
LIMIT 750;

-- 1000 elementos
SET enable_seqscan = false;
EXPLAIN ANALYZE
SELECT title, description, ts_rank_cd(weighted_tsv, query) AS rank
FROM film, to_tsquery('english', 'Man | Woman') query
WHERE query @@ weighted_tsv
ORDER BY rank DESC
LIMIT 1000;


-- PREGUNTA 3: 


CREATE TABLE articles (
    article_id SERIAL PRIMARY KEY,
    date DATE,
    year INT,
    month INT,
    day INT,
    author TEXT,
    title TEXT,
    article TEXT,
    url TEXT,
    section TEXT,
    publication TEXT
);

COPY articles(date, year, month, day, author, title, article, url, section, publication)
FROM 'C:\Users\vilch\OneDrive\Escritorio\5TH SEMESTER CODE\DATABASE II\LABS\LAB 8.1\news.csv'
DELIMITER ','
CSV HEADER;

-- Agregar columnas para tsvector
ALTER TABLE articles ADD COLUMN weighted_tsv tsvector;
ALTER TABLE articles ADD COLUMN weighted_tsv2 tsvector;

-- Almacenar y poblar los vectores de documentos
UPDATE articles SET
    weighted_tsv = x.weighted_tsv,
    weighted_tsv2 = x.weighted_tsv
FROM (
    SELECT article_id,
           setweight(to_tsvector('english', COALESCE(title, '')), 'A') ||
           setweight(to_tsvector('english', COALESCE(article, '')), 'B') AS weighted_tsv
    FROM articles
) AS x
WHERE x.article_id = articles.article_id;

-- Crear índice GIN en weighted_tsv2
CREATE INDEX weighted_tsv_idx ON articles USING GIN (weighted_tsv2);


-- Consultas sin índice

-- 1000 elementos
EXPLAIN ANALYZE
SELECT title, article
FROM (
    SELECT *, (title || article) AS texto
    FROM articles
    LIMIT 1000
) T
WHERE T.texto ILIKE '%man%' OR T.texto ILIKE '%woman%';

-- 10000 elementos
EXPLAIN ANALYZE
SELECT title, article
FROM (
    SELECT *, (title || article) AS texto
    FROM articles
    LIMIT 10000
) T
WHERE T.texto ILIKE '%man%' OR T.texto ILIKE '%woman%';

-- 100000 elementos
EXPLAIN ANALYZE
SELECT title, article
FROM (
    SELECT *, (title || article) AS texto
    FROM articles
    LIMIT 100000
) T
WHERE T.texto ILIKE '%man%' OR T.texto ILIKE '%woman%';

-- 1000000 elementos
EXPLAIN ANALYZE
SELECT title, article
FROM (
    SELECT *, (title || article) AS texto
    FROM articles
    LIMIT 1000000
) T
WHERE T.texto ILIKE '%man%' OR T.texto ILIKE '%woman%';

-- 2489643 elementos (todos los elementos si tienes este número de filas)
EXPLAIN ANALYZE
SELECT title, article
FROM (
    SELECT *, (title || article) AS texto
    FROM articles
    LIMIT 2489643
) T
WHERE T.texto ILIKE '%man%' OR T.texto ILIKE '%woman%';

-- Consultas con índice

-- 1000 elementos
SET enable_seqscan = false;
EXPLAIN ANALYZE
SELECT title, article, ts_rank_cd(weighted_tsv, query) AS rank
FROM articles, to_tsquery('english', 'Man | Woman') query
WHERE query @@ weighted_tsv
ORDER BY rank DESC
LIMIT 1000;

-- 10000 elementos
SET enable_seqscan = false;
EXPLAIN ANALYZE
SELECT title, article, ts_rank_cd(weighted_tsv, query) AS rank
FROM articles, to_tsquery('english', 'Man | Woman') query
WHERE query @@ weighted_tsv
ORDER BY rank DESC
LIMIT 10000;

-- 100000 elementos
SET enable_seqscan = false;
EXPLAIN ANALYZE
SELECT title, article, ts_rank_cd(weighted_tsv, query) AS rank
FROM articles, to_tsquery('english', 'Man | Woman') query
WHERE query @@ weighted_tsv
ORDER BY rank DESC
LIMIT 100000;

-- 1000000 elementos
SET enable_seqscan = false;
EXPLAIN ANALYZE
SELECT title, article, ts_rank_cd(weighted_tsv, query) AS rank
FROM articles, to_tsquery('english', 'Man | Woman') query
WHERE query @@ weighted_tsv
ORDER BY rank DESC
LIMIT 1000000;

-- 2489643 elementos (todos los elementos si tienes este número de filas)
SET enable_seqscan = false;
EXPLAIN ANALYZE
SELECT title, article, ts_rank_cd(weighted_tsv, query) AS rank
FROM articles, to_tsquery('english', 'Man | Woman') query
WHERE query @@ weighted_tsv
ORDER BY rank DESC
LIMIT 2489643;


