/* P0 */

DROP TABLE IF EXISTS Alumnos CASCADE;
DROP TABLE IF EXISTS talumnos CASCADE;

CREATE TABLE alumnos (
    dni varchar(8),
    nombre varchar(30),
    ciudad varchar(30),
    grupo varchar(1),
    promedio float,
    edad int,
    sexo varchar(1)
) PARTITION BY LIST (ciudad);

CREATE TABLE talumnos (
    dni varchar(8),
    nombre varchar(30),
    ciudad varchar(30),
    grupo varchar(1),
    promedio float,
    edad int,
    sexo varchar(1)
);

SET enable_partition_pruning = on;

CREATE TABLE alumnos_lima
PARTITION OF alumnos
FOR VALUES IN ('Lima');

CREATE TABLE alumnos_callao
PARTITION OF alumnos
FOR VALUES IN ('Callao');


/* P1 */
CREATE OR REPLACE FUNCTION create_partition_and_insert() RETURNS trigger AS
$BODY$
DECLARE
    partition_city TEXT;
    partition TEXT;
    parent_table TEXT := 'alumnos';
    partition_exists BOOLEAN;
BEGIN
    RAISE NOTICE 'Inserting row into partitioned table. The values are: %', NEW;
    partition_city := NEW.ciudad;
    partition := parent_table || '_' || lower(partition_city);

    SELECT EXISTS (
        SELECT 1 FROM pg_inherits
        JOIN pg_class parent ON pg_inherits.inhparent = parent.oid
        JOIN pg_class child ON pg_inherits.inhrelid = child.oid
        WHERE parent.relname = parent_table
        AND child.relname = partition
    ) INTO partition_exists;

    IF NOT partition_exists THEN
        EXECUTE 'CREATE TABLE ' || partition || ' PARTITION OF ' || parent_table || ' FOR VALUES IN (' || quote_literal(partition_city) || ');';
    END IF;

    EXECUTE 'INSERT INTO ' || partition || ' VALUES (' || quote_literal(NEW.dni) || ', ' ||
                                               quote_literal(NEW.nombre) || ', ' ||
                                               quote_literal(NEW.ciudad) || ', ' ||
                                               quote_literal(NEW.grupo) || ', ' ||
                                               NEW.promedio || ', ' ||
                                               NEW.edad || ', ' ||
                                               quote_literal(NEW.sexo) || ');';
    RETURN NULL;
END
$BODY$
LANGUAGE plpgsql VOLATILE
COST 100;

CREATE OR REPLACE TRIGGER partition_insert_trigger
BEFORE INSERT ON talumnos
FOR EACH ROW EXECUTE PROCEDURE create_partition_and_insert();

INSERT INTO talumnos VALUES ('72123456', 'Juan Perez', 'Lima', 'A', 15.5, 20, 'M');
INSERT INTO talumnos VALUES ('72123457', 'Maria Perez', 'Callao', 'B', 14.5, 21, 'F');
INSERT INTO talumnos VALUES ('72123458', 'Jose Perez', 'Cuzco', 'A', 16.5, 22, 'M');
INSERT INTO talumnos VALUES ('72123459', 'Ana Perez', 'Arequipa', 'B', 17.5, 23, 'F');
INSERT INTO talumnos VALUES ('72123460', 'Pedro Perez', 'Tacna', 'A', 18.5, 24, 'M');

/*
#### Triggers en Tablas Particionadas

- En PostgreSQL, se pueden definir triggers tanto en la tabla principal como en las particiones individuales.
- Un trigger definido en la tabla principal se aplicará automáticamente a todas sus particiones.
- También es posible definir triggers específicos para particiones individuales si se requiere un comportamiento particular para una partición específica.
- Sin embargo, los triggers pueden impactar el rendimiento de las operaciones de inserción, actualización o eliminación, especialmente en tablas grandes o con muchas particiones.

#### Claves Primarias en Tablas Particionadas

- PostgreSQL no permite definir una clave primaria directamente en la tabla principal de una tabla particionada. Esto se debe a que las restricciones de unicidad y claves primarias deben gestionarse a nivel de partición.
- No obstante, sí se pueden definir claves primarias en cada partición individualmente.
- Para asegurar la unicidad global en toda la tabla particionada, se pueden utilizar restricciones de exclusión junto con triggers adicionales que validen la unicidad a través de todas las particiones.
- Alternativamente, se pueden definir índices únicos en cada partición. Sin embargo, esto no garantiza la unicidad a nivel global sin mecanismos adicionales.
*/

/* P2 */

CREATE EXTENSION postgres_fdw;

CREATE SERVER remote_server
FOREIGN DATA WRAPPER postgres_fdw
OPTIONS (host 'localhost', port '5433', dbname 'remote_db');

CREATE USER MAPPING FOR postgres
SERVER remote_server
OPTIONS (user 'remote_user', password 'remote_password');

IMPORT FOREIGN SCHEMA public
    LIMIT TO (alumnos_remoto, talumnos_remoto)
    FROM SERVER remote_server
    INTO public;

CREATE OR REPLACE FUNCTION sync_insert_to_remote() RETURNS trigger AS
$BODY$
BEGIN
    RAISE NOTICE 'Inserting row into remote server. The values are: %', NEW;
    EXECUTE 'INSERT INTO talumnos_remoto (dni, nombre, ciudad, grupo, promedio, edad, sexo) VALUES (' || quote_literal(NEW.dni) || ', ' ||
                                              quote_literal(NEW.nombre) || ', ' ||
                                              quote_literal(NEW.ciudad) || ', ' ||
                                              quote_literal(NEW.grupo) || ', ' ||
                                              NEW.promedio || ', ' ||
                                              NEW.edad || ', ' ||
                                              quote_literal(NEW.sexo) || ');';
    RETURN NEW;
END
$BODY$
LANGUAGE plpgsql VOLATILE
COST 100;

CREATE OR REPLACE TRIGGER sync_insert_trigger
AFTER INSERT ON talumnos
FOR EACH ROW EXECUTE PROCEDURE sync_insert_to_remote();

COPY talumnos(dni, nombre, ciudad, grupo, promedio, edad, sexo)
FROM 'C:\Users\vilch\Desktop\dataset.csv'
DELIMITER ','
CSV HEADER;


-- CONSULTA 1
SELECT ciudad, COUNT(*) as total_alumnos
FROM (
    SELECT ciudad FROM alumnos
    UNION ALL
    SELECT ciudad FROM alumnos_remoto
) AS combined
GROUP BY ciudad;

-- CONSULTA 2
SELECT dni, nombre, ciudad, grupo, promedio, edad, sexo
FROM talumnos
WHERE promedio > 15
UNION ALL
SELECT dni, nombre, ciudad, grupo, promedio, edad, sexo
FROM talumnos_remoto
WHERE promedio > 15;

--CONSULTA 3

SELECT ciudad, COUNT(*) as num_alumnos
FROM (
    SELECT ciudad FROM talumnos
    UNION ALL
    SELECT ciudad FROM talumnos_remoto
) as all_students
GROUP BY ciudad;









