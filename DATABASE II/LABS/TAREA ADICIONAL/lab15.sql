SET search_path TO lab15;

-- Crear la tabla venta particionada
CREATE TABLE venta (
    IdVenta SERIAL,
    DNI_Cliente INT,
    FechaVenta DATE,
    CodLocal VARCHAR(5),
    ImporteTotal FLOAT,
    IdEmpleado VARCHAR(6)
) PARTITION BY RANGE (FechaVenta);

-- Crear particiones para la tabla venta
CREATE TABLE venta_2000_2008 PARTITION OF venta
    FOR VALUES FROM ('2000-01-01') TO ('2009-01-01');

CREATE TABLE venta_2009_2016 PARTITION OF venta
    FOR VALUES FROM ('2009-01-01') TO ('2017-01-01');

CREATE TABLE venta_2017_2024 PARTITION OF venta
    FOR VALUES FROM ('2017-01-01') TO ('2025-01-01');

-- Crear tabla reclamo particionada por lista en CodLocal
CREATE TABLE reclamo (
    IdReclamo SERIAL,
    DNI_Cliente INT,
    FechaReclamo DATE,
    CodLocal VARCHAR(5),
    Descripcion VARCHAR(100),
    Estado VARCHAR(10)
) PARTITION BY LIST (CodLocal);

-- Crear particiones para la tabla reclamo
CREATE TABLE reclamo_loc1 PARTITION OF reclamo
    FOR VALUES IN ('LOC01', 'LOC02', 'LOC03', 'LOC04');

CREATE TABLE reclamo_loc2 PARTITION OF reclamo
    FOR VALUES IN ('LOC05', 'LOC06', 'LOC07', 'LOC08');

CREATE TABLE reclamo_loc3 PARTITION OF reclamo
    FOR VALUES IN ('LOC09', 'LOC10', 'LOC11', 'LOC12');

-- Verificando la distribución de los datos para cada una de las tablas
SELECT 'venta_2000_2008' AS tabla, COUNT(*) AS total_filas
FROM venta_2000_2008
UNION ALL
SELECT 'venta_2009_2016' AS tabla, COUNT(*) AS total_filas
FROM venta_2009_2016
UNION ALL
SELECT 'venta_2017_2024' AS tabla, COUNT(*) AS total_filas
FROM venta_2017_2024;

SELECT 'reclamo_loc1' AS tabla, COUNT(*) AS total_filas
FROM reclamo_loc1
UNION ALL
SELECT 'reclamo_loc2' AS tabla, COUNT(*) AS total_filas
FROM reclamo_loc2
UNION ALL
SELECT 'reclamo_loc3' AS tabla, COUNT(*) AS total_filas
FROM reclamo_loc3;

-- Consultas a las tablas
SELECT *
FROM venta
ORDER BY ImporteTotal DESC;

SELECT DISTINCT DNI_Cliente
FROM venta;

SELECT CodLocal, AVG(ImporteTotal) AS promedio_importe
FROM venta
GROUP BY CodLocal
ORDER BY CodLocal;

SELECT R.*
FROM reclamo R
JOIN venta V
ON R.DNI_Cliente = V.DNI_Cliente;

-- P1. Algoritmos distribuídos para consultas
/* Consulta 1
    El atributo ImporteTotal se generó en el rango de 1 a 1000,
    por lo que se tomó el vector de partición [333,666] en el atributo ImporteTotal
*/

CREATE OR REPLACE FUNCTION consulta1()
RETURNS TABLE (
    IdVenta INT,
    DNI_Cliente INT,
    FechaVenta DATE,
    CodLocal VARCHAR(5),
    ImporteTotal FLOAT,
    IdEmpleado VARCHAR(6)
) AS $$
BEGIN
    CREATE TEMPORARY TABLE temp_venta1 AS
        SELECT V.*
        FROM venta_2000_2008 V
        WHERE V.ImporteTotal < 333
        UNION ALL
        SELECT V.*
        FROM venta_2009_2016 V
        WHERE V.ImporteTotal < 333
        UNION ALL
        SELECT V.*
        FROM venta_2017_2024 V
        WHERE V.ImporteTotal < 333;

    CREATE TEMPORARY TABLE temp_venta2 AS
        SELECT V.*
        FROM venta_2000_2008 V
        WHERE V.ImporteTotal >= 333 AND V.ImporteTotal < 666
        UNION ALL
        SELECT V.*
        FROM venta_2009_2016 V
        WHERE V.ImporteTotal >= 333 AND V.ImporteTotal < 666
        UNION ALL
        SELECT V.*
        FROM venta_2017_2024 V
        WHERE V.ImporteTotal >= 333 AND V.ImporteTotal < 666;

    CREATE TEMPORARY TABLE temp_venta3 AS
        SELECT V.*
        FROM venta_2000_2008 V
        WHERE V.ImporteTotal >= 666
        UNION ALL
        SELECT V.*
        FROM venta_2009_2016 V
        WHERE V.ImporteTotal >= 666
        UNION ALL
        SELECT V.*
        FROM venta_2017_2024 V
        WHERE V.ImporteTotal >= 666;

    CREATE TEMPORARY TABLE temp_venta4 AS
        SELECT *
        FROM temp_venta1
        ORDER BY ImporteTotal DESC;

    CREATE TEMPORARY TABLE temp_venta5 AS
        SELECT *
        FROM temp_venta2
        ORDER BY ImporteTotal DESC;

    CREATE TEMPORARY TABLE temp_venta6 AS
        SELECT *
        FROM temp_venta3
        ORDER BY ImporteTotal DESC;

    RETURN QUERY
        SELECT *
        FROM temp_venta6
        UNION ALL
        SELECT *
        FROM temp_venta5
        UNION ALL
        SELECT *
        FROM temp_venta4;

    DROP TABLE IF EXISTS temp_venta1;
    DROP TABLE IF EXISTS temp_venta2;
    DROP TABLE IF EXISTS temp_venta3;
    DROP TABLE IF EXISTS temp_venta4;
    DROP TABLE IF EXISTS temp_venta5;
    DROP TABLE IF EXISTS temp_venta6;
END;
$$ LANGUAGE plpgsql;

SELECT *
FROM consulta1();

/* Consulta 2
    El atributo DNI_Cliente se generó en el rango de 70000000 a 77500000
   por lo que se tomó el vector de partición [72500000, 75000000] en el atributo DNI_Cliente
*/

CREATE OR REPLACE FUNCTION consulta2()
RETURNS TABLE (
    DNI_Cliente INT
) AS $$
BEGIN
    CREATE TEMPORARY TABLE temp_venta1 AS
        SELECT V.DNI_Cliente
        FROM venta_2000_2008 V
        WHERE V.DNI_Cliente < 72500000
        UNION ALL
        SELECT V.DNI_Cliente
        FROM venta_2009_2016 V
        WHERE V.DNI_Cliente < 72500000
        UNION ALL
        SELECT V.DNI_Cliente
        FROM venta_2017_2024 V
        WHERE V.DNI_Cliente < 72500000;

    CREATE TEMPORARY TABLE temp_venta2 AS
        SELECT V.DNI_Cliente
        FROM venta_2000_2008 V
        WHERE V.DNI_Cliente >= 72500000 AND V.DNI_Cliente < 75000000
        UNION ALL
        SELECT V.DNI_Cliente
        FROM venta_2009_2016 V
        WHERE V.DNI_Cliente >= 72500000 AND V.DNI_Cliente < 75000000
        UNION ALL
        SELECT V.DNI_Cliente
        FROM venta_2017_2024 V
        WHERE V.DNI_Cliente >= 72500000 AND V.DNI_Cliente < 75000000;

    CREATE TEMPORARY TABLE temp_venta3 AS
        SELECT V.DNI_Cliente
        FROM venta_2000_2008 V
        WHERE V.DNI_Cliente >= 75000000
        UNION ALL
        SELECT V.DNI_Cliente
        FROM venta_2009_2016 V
        WHERE V.DNI_Cliente >= 75000000
        UNION ALL
        SELECT V.DNI_Cliente
        FROM venta_2017_2024 V
        WHERE V.DNI_Cliente >= 75000000;

    CREATE TEMPORARY TABLE temp_venta4 AS
        SELECT DISTINCT V.DNI_Cliente
        FROM temp_venta1 V;

    CREATE TEMPORARY TABLE temp_venta5 AS
        SELECT DISTINCT V.DNI_Cliente
        FROM temp_venta2 V;

    CREATE TEMPORARY TABLE temp_venta6 AS
        SELECT DISTINCT V.DNI_Cliente
        FROM temp_venta3 V;

    RETURN QUERY
        SELECT *
        FROM temp_venta6
        UNION ALL
        SELECT *
        FROM temp_venta5
        UNION ALL
        SELECT *
        FROM temp_venta4;

    DROP TABLE IF EXISTS temp_venta1;
    DROP TABLE IF EXISTS temp_venta2;
    DROP TABLE IF EXISTS temp_venta3;
    DROP TABLE IF EXISTS temp_venta4;
    DROP TABLE IF EXISTS temp_venta5;
    DROP TABLE IF EXISTS temp_venta6;
END;
$$ LANGUAGE plpgsql;

SELECT *
FROM consulta2();

/*
    Consulta 3
    El atributo CodLocal se generó con los valores LOC01 - LOC12,
    por lo que se dividió en 3 grupos de 4 valores cada uno: [LOC01 a LOC04], [LOC05 a LOC08], [LOC09 a LOC12]
*/

CREATE OR REPLACE FUNCTION consulta3()
RETURNS TABLE (
    CodLocal VARCHAR(5),
    PromedioImporte FLOAT
) AS $$
BEGIN
    CREATE TEMPORARY TABLE temp_venta1 AS
        SELECT V.CodLocal, SUM(V.ImporteTotal) AS SumaImporte, COUNT(*) AS CantVentas
        FROM venta_2000_2008 V
        WHERE V.CodLocal IN ('LOC01', 'LOC02', 'LOC03', 'LOC04')
        GROUP BY V.CodLocal
        UNION ALL
        SELECT V.CodLocal, SUM(V.ImporteTotal) AS SumaImporte, COUNT(*) AS CantVentas
        FROM venta_2009_2016 V
        WHERE V.CodLocal IN ('LOC01', 'LOC02', 'LOC03', 'LOC04')
        GROUP BY V.CodLocal
        UNION ALL
        SELECT V.CodLocal, SUM(V.ImporteTotal) AS SumaImporte, COUNT(*) AS CantVentas
        FROM venta_2017_2024 V
        WHERE V.CodLocal IN ('LOC01', 'LOC02', 'LOC03', 'LOC04')
        GROUP BY V.CodLocal;

    CREATE TEMPORARY TABLE temp_venta2 AS
        SELECT V.CodLocal, SUM(V.ImporteTotal) AS SumaImporte, COUNT(*) AS CantVentas
        FROM venta_2000_2008 V
        WHERE V.CodLocal IN ('LOC05', 'LOC06', 'LOC07', 'LOC08')
        GROUP BY V.CodLocal
        UNION ALL
        SELECT V.CodLocal, SUM(V.ImporteTotal) AS SumaImporte, COUNT(*) AS CantVentas
        FROM venta_2009_2016 V
        WHERE V.CodLocal IN ('LOC05', 'LOC06', 'LOC07', 'LOC08')
        GROUP BY V.CodLocal
        UNION ALL
        SELECT V.CodLocal, SUM(V.ImporteTotal) AS SumaImporte, COUNT(*) AS CantVentas
        FROM venta_2017_2024 V
        WHERE V.CodLocal IN ('LOC05', 'LOC06', 'LOC07', 'LOC08')
        GROUP BY V.CodLocal;

    CREATE TEMPORARY TABLE temp_venta3 AS
        SELECT V.CodLocal, SUM(V.ImporteTotal) AS SumaImporte, COUNT(*) AS CantVentas
        FROM venta_2000_2008 V
        WHERE V.CodLocal IN ('LOC09', 'LOC10', 'LOC11', 'LOC12')
        GROUP BY V.CodLocal
        UNION ALL
        SELECT V.CodLocal, SUM(V.ImporteTotal) AS SumaImporte, COUNT(*) AS CantVentas
        FROM venta_2009_2016 V
        WHERE V.CodLocal IN ('LOC09', 'LOC10', 'LOC11', 'LOC12')
        GROUP BY V.CodLocal
        UNION ALL
        SELECT V.CodLocal, SUM(V.ImporteTotal) AS SumaImporte, COUNT(*) AS CantVentas
        FROM venta_2017_2024 V
        WHERE V.CodLocal IN ('LOC09', 'LOC10', 'LOC11', 'LOC12')
        GROUP BY V.CodLocal;

    CREATE TEMPORARY TABLE temp_venta4 AS
        SELECT V.CodLocal, SUM(SumaImporte)/SUM(CantVentas)
        FROM temp_venta1 V
        GROUP BY V.CodLocal;

    CREATE TEMPORARY TABLE temp_venta5 AS
        SELECT V.CodLocal, SUM(SumaImporte)/SUM(CantVentas)
        FROM temp_venta2 V
        GROUP BY V.CodLocal;

    CREATE TEMPORARY TABLE temp_venta6 AS
        SELECT V.CodLocal, SUM(SumaImporte)/SUM(CantVentas)
        FROM temp_venta3 V
        GROUP BY V.CodLocal;

    RETURN QUERY
        SELECT *
        FROM temp_venta6
        UNION ALL
        SELECT *
        FROM temp_venta5
        UNION ALL
        SELECT *
        FROM temp_venta4;

    DROP TABLE IF EXISTS temp_venta1;
    DROP TABLE IF EXISTS temp_venta2;
    DROP TABLE IF EXISTS temp_venta3;
    DROP TABLE IF EXISTS temp_venta4;
    DROP TABLE IF EXISTS temp_venta5;
    DROP TABLE IF EXISTS temp_venta6;
END;
$$ LANGUAGE plpgsql;

SELECT *
FROM consulta3()
ORDER BY CodLocal;

/*
    Consulta 4
    El atributo DNI_Cliente se generó en el rango de 70000000 a 77500000
    por lo que se tomó el vector de partición [72500000, 75000000] en el atributo DNI_Cliente
*/

CREATE OR REPLACE FUNCTION consulta4()
RETURNS TABLE (
    IdReclamo INT,
    DNI_Cliente INT,
    FechaReclamo DATE,
    CodLocal VARCHAR(5),
    Descripcion VARCHAR(100),
    Estado VARCHAR(10)
) AS $$
BEGIN
    CREATE TEMPORARY TABLE temp_venta1 AS
        SELECT V.DNI_Cliente
        FROM venta_2000_2008 V
        WHERE V.DNI_Cliente < 72500000
        UNION ALL
        SELECT V.DNI_Cliente
        FROM venta_2009_2016 V
        WHERE V.DNI_Cliente < 72500000
        UNION ALL
        SELECT V.DNI_Cliente
        FROM venta_2017_2024 V
        WHERE V.DNI_Cliente < 72500000;

    CREATE TEMPORARY TABLE temp_venta2 AS
        SELECT V.DNI_Cliente
        FROM venta_2000_2008 V
        WHERE V.DNI_Cliente >= 72500000 AND V.DNI_Cliente < 75000000
        UNION ALL
        SELECT V.DNI_Cliente
        FROM venta_2009_2016 V
        WHERE V.DNI_Cliente >= 72500000 AND V.DNI_Cliente < 75000000
        UNION ALL
        SELECT V.DNI_Cliente
        FROM venta_2017_2024 V
        WHERE V.DNI_Cliente >= 72500000 AND V.DNI_Cliente < 75000000;

    CREATE TEMPORARY TABLE temp_venta3 AS
        SELECT V.DNI_Cliente
        FROM venta_2000_2008 V
        WHERE V.DNI_Cliente >= 75000000
        UNION ALL
        SELECT V.DNI_Cliente
        FROM venta_2009_2016 V
        WHERE V.DNI_Cliente >= 75000000
        UNION ALL
        SELECT V.DNI_Cliente
        FROM venta_2017_2024 V
        WHERE V.DNI_Cliente >= 75000000;

    CREATE TEMPORARY TABLE temp_reclamo1 AS
        SELECT R.*
        FROM reclamo_loc1 R
        WHERE R.DNI_Cliente < 72500000
        UNION ALL
        SELECT R.*
        FROM reclamo_loc2 R
        WHERE R.DNI_Cliente < 72500000
        UNION ALL
        SELECT R.*
        FROM reclamo_loc3 R
        WHERE R.DNI_Cliente < 72500000;

    CREATE TEMPORARY TABLE temp_reclamo2 AS
        SELECT R.*
        FROM reclamo_loc1 R
        WHERE R.DNI_Cliente >= 72500000 AND R.DNI_Cliente < 75000000
        UNION ALL
        SELECT R.*
        FROM reclamo_loc2 R
        WHERE R.DNI_Cliente >= 72500000 AND R.DNI_Cliente < 75000000
        UNION ALL
        SELECT R.*
        FROM reclamo_loc3 R
        WHERE R.DNI_Cliente >= 72500000 AND R.DNI_Cliente < 75000000;

    CREATE TEMPORARY TABLE temp_reclamo3 AS
        SELECT R.*
        FROM reclamo_loc1 R
        WHERE R.DNI_Cliente >= 75000000
        UNION ALL
        SELECT R.*
        FROM reclamo_loc2 R
        WHERE R.DNI_Cliente >= 75000000
        UNION ALL
        SELECT R.*
        FROM reclamo_loc3 R
        WHERE R.DNI_Cliente >= 75000000;

    CREATE TEMPORARY TABLE temp_join1 AS
        SELECT R.*
        FROM temp_reclamo1 R
        JOIN temp_venta1 V
        ON R.DNI_Cliente = V.DNI_Cliente;

    CREATE TEMPORARY TABLE temp_join2 AS
        SELECT R.*
        FROM temp_reclamo2 R
        JOIN temp_venta2 V
        ON R.DNI_Cliente = V.DNI_Cliente;

    CREATE TEMPORARY TABLE temp_join3 AS
        SELECT R.*
        FROM temp_reclamo3 R
        JOIN temp_venta3 V
        ON R.DNI_Cliente = V.DNI_Cliente;

    RETURN QUERY
        SELECT *
        FROM temp_join1
        UNION ALL
        SELECT *
        FROM temp_join2
        UNION ALL
        SELECT *
        FROM temp_join3;

    DROP TABLE IF EXISTS temp_venta1;
    DROP TABLE IF EXISTS temp_venta2;
    DROP TABLE IF EXISTS temp_venta3;
    DROP TABLE IF EXISTS temp_reclamo1;
    DROP TABLE IF EXISTS temp_reclamo2;
    DROP TABLE IF EXISTS temp_reclamo3;
    DROP TABLE IF EXISTS temp_join1;
    DROP TABLE IF EXISTS temp_join2;
    DROP TABLE IF EXISTS temp_join3;
END;
$$ LANGUAGE plpgsql;

SELECT *
FROM consulta4();

/*
DROP TABLE venta;
DROP TABLE reclamo;

DELETE FROM venta;
DELETE FROM reclamo;


EXPLAIN ANALYSE
SELECT CodLocal, COUNT(*) AS cant
FROM venta
GROUP BY CodLocal
ORDER BY cant DESC;

EXPLAIN ANALYSE
SELECT *
FROM venta
WHERE FechaVenta < '2010-01-01';

SELECT COUNT(*) FROM venta;

SELECT CodLocal, COUNT(*) AS cant
FROM reclamo
GROUP BY CodLocal
ORDER BY cant DESC;

*/

