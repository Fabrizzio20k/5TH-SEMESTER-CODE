CREATE TABLE alumnos_remoto (
    dni varchar(8),
    nombre varchar(30),
    ciudad varchar(30),
    grupo varchar(1),
    promedio float,
    edad int,
    sexo varchar(1)
) PARTITION BY LIST (ciudad);

CREATE TABLE alumnos_remoto_lima
PARTITION OF alumnos_remoto
FOR VALUES IN ('Lima');

CREATE TABLE alumnos_remoto_callao
PARTITION OF alumnos_remoto
FOR VALUES IN ('Callao');

CREATE TABLE talumnos_remoto (
    dni varchar(8),
    nombre varchar(30),
    ciudad varchar(30),
    grupo varchar(1),
    promedio float,
    edad int,
    sexo varchar(1)
);

CREATE OR REPLACE FUNCTION create_partition_and_insert_remote() RETURNS trigger AS
$BODY$
DECLARE
    partition_city TEXT;
    partition TEXT;
    parent_table TEXT := 'alumnos_remoto';
    partition_exists BOOLEAN;
BEGIN
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
        EXECUTE 'CREATE TABLE ' || quote_ident(partition) || ' PARTITION OF ' || quote_ident(parent_table) || ' FOR VALUES IN (' || quote_literal(partition_city) || ');';
    END IF;

    EXECUTE 'INSERT INTO ' || quote_ident(partition) || ' VALUES (' || quote_literal(NEW.dni) || ', ' ||
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

CREATE OR REPLACE TRIGGER partition_insert_trigger_remote
BEFORE INSERT ON talumnos_remoto
FOR EACH ROW EXECUTE PROCEDURE create_partition_and_insert_remote();
