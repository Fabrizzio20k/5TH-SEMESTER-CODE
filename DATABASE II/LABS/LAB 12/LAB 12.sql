CREATE TABLE employees (
  emp_no int,
  birth_date date,
  first_name varchar(14),
  last_name varchar(16),
  gender character(1),
  hire_date date,
  dept_no varchar(5),
  from_date date
);

CREATE TABLE salaries (
  emp_no int,
  salary int,
  from_date date,
  to_date date
);

COPY employees(emp_no, birth_date, first_name, last_name, gender, hire_date, dept_no, from_date)
FROM 'C:\\Users\\vilch\\OneDrive\\Escritorio\\5TH SEMESTER CODE\\DATABASE II\\LABS\\LAB 12\\employees.csv'
DELIMITER ','
CSV HEADER;

COPY salaries(emp_no, salary, from_date, to_date)
FROM 'C:\\Users\\vilch\\OneDrive\\Escritorio\\5TH SEMESTER CODE\\DATABASE II\\LABS\\LAB 12\\salaries.csv'
DELIMITER ','
CSV HEADER;

select dept_no, count(*) from employees group by dept_no;

select dept_no, count(*) from employees where dept_no in ('d005', 'd004', 'd007') group by dept_no;


-- PREGUNTA 1: Partition by list

CREATE TABLE employees1 (
  emp_no int,
  birth_date date,
  first_name varchar(14),
  last_name varchar(16),
  gender character(1),
  hire_date date,
  dept_no varchar(5),
  from_date date
) PARTITION BY LIST (dept_no);

CREATE TABLE employees1_d005 PARTITION OF employees1 FOR VALUES IN ('d001', 'd005');
CREATE TABLE employees1_d004 PARTITION OF employees1 FOR VALUES IN ('d002', 'd003', 'd004');
CREATE TABLE employees1_d007 PARTITION OF employees1 FOR VALUES IN ('d006', 'd007', 'd008', 'd009');
CREATE TABLE employees1_default PARTITION OF employees1 DEFAULT;


COPY employees1 (emp_no, birth_date, first_name, last_name, gender, hire_date, dept_no, from_date)
FROM 'C:\\Users\\vilch\\OneDrive\\Escritorio\\5TH SEMESTER CODE\\DATABASE II\\LABS\\LAB 12\\employees.csv'
DELIMITER ','
CSV HEADER;

-- Activar la poda de particiones
SET enable_partition_pruning = on;

-- Consultas para el atributo dept_no en la tabla employees
EXPLAIN ANALYZE SELECT * FROM employees WHERE dept_no = 'd005';
EXPLAIN ANALYZE SELECT * FROM employees WHERE dept_no = 'd004';
EXPLAIN ANALYZE SELECT * FROM employees WHERE dept_no = 'd007';

-- Consultas para el atributo dept_no en la tabla employees1
EXPLAIN ANALYZE SELECT * FROM employees1 WHERE dept_no = 'd005';
EXPLAIN ANALYZE SELECT * FROM employees1 WHERE dept_no = 'd004';
EXPLAIN ANALYZE SELECT * FROM employees1 WHERE dept_no = 'd007';

-- PREGUNTA 2: Partition by range

SELECT date_part('year', hire_date), count(*) FROM employees GROUP BY date_part('year', hire_date) ORDER BY date_part('year', hire_date);

CREATE TABLE employees2 (
  emp_no int,
  birth_date date,
  first_name varchar(14),
  last_name varchar(16),
  gender character(1),
  hire_date date,
  dept_no varchar(5),
  from_date date
) PARTITION BY RANGE (date_part('year', hire_date));

CREATE TABLE employees2_before_1988 PARTITION OF employees2
    FOR VALUES FROM (MINVALUE) TO (1988);

CREATE TABLE employees2_1988_1994 PARTITION OF employees2
    FOR VALUES FROM (1988) TO (1995);

CREATE TABLE employees2_after_1994 PARTITION OF employees2
    FOR VALUES FROM (1995) TO (MAXVALUE);


COPY employees2 (emp_no, birth_date, first_name, last_name, gender, hire_date, dept_no, from_date)
FROM 'C:\\Users\\vilch\\OneDrive\\Escritorio\\5TH SEMESTER CODE\\DATABASE II\\LABS\\LAB 12\\employees.csv'
DELIMITER ','
CSV HEADER;

-- Activar la poda de particiones
SET enable_partition_pruning = on;

-- Consultas para el atributo hire_date en la tabla employees
EXPLAIN ANALYZE SELECT * FROM employees WHERE date_part('year', hire_date) BETWEEN 1985 AND 1987;
EXPLAIN ANALYZE SELECT * FROM employees WHERE date_part('year', hire_date) BETWEEN 1988 AND 1994;
EXPLAIN ANALYZE SELECT * FROM employees WHERE date_part('year', hire_date) BETWEEN 1995 AND 2000;

-- Consultas para el atributo hire_date en la tabla employees2
EXPLAIN ANALYZE SELECT * FROM employees2 WHERE date_part('year', hire_date) BETWEEN 1985 AND 1987;
EXPLAIN ANALYZE SELECT * FROM employees2 WHERE date_part('year', hire_date) BETWEEN 1988 AND 1994;
EXPLAIN ANALYZE SELECT * FROM employees2 WHERE date_part('year', hire_date) BETWEEN 1995 AND 2000;

-- PREGUNTA 2 - PARTE 2: Partition by range with btree index

CREATE TABLE employees3 (
  emp_no int,
  birth_date date,
  first_name varchar(14),
  last_name varchar(16),
  gender character(1),
  hire_date date,
  dept_no varchar(5),
  from_date date
);

COPY employees3(emp_no, birth_date, first_name, last_name, gender, hire_date, dept_no, from_date)
FROM 'C:\\Users\\vilch\\OneDrive\\Escritorio\\5TH SEMESTER CODE\\DATABASE II\\LABS\\LAB 12\\employees.csv'
DELIMITER ','
CSV HEADER;

-- Índice en la tabla employees
CREATE INDEX idx_employees_hire_date ON employees3 USING btree (date_part('year', hire_date));

-- Índice distribuido en la tabla employees2 particionada
CREATE INDEX idx_employees2_hire_date ON employees2 USING btree (date_part('year', hire_date));

-- Consultas en la tabla employees
EXPLAIN ANALYZE SELECT * FROM employees3 WHERE date_part('year', hire_date) BETWEEN 1985 AND 1987;
EXPLAIN ANALYZE SELECT * FROM employees3 WHERE date_part('year', hire_date) BETWEEN 1988 AND 1994;
EXPLAIN ANALYZE SELECT * FROM employees3 WHERE date_part('year', hire_date) BETWEEN 1995 AND 2000;

-- Consultas en la tabla employees2
EXPLAIN ANALYZE SELECT * FROM employees2 WHERE date_part('year', hire_date) BETWEEN 1985 AND 1987;
EXPLAIN ANALYZE SELECT * FROM employees2 WHERE date_part('year', hire_date) BETWEEN 1988 AND 1994;
EXPLAIN ANALYZE SELECT * FROM employees2 WHERE date_part('year', hire_date) BETWEEN 1995 AND 2000;

-- PREGUNTA 3: Partition by two columns

DROP TABLE IF EXISTS employees3;

ALTER TABLE employees ADD COLUMN salary int;

UPDATE employees e
SET salary = s.salary
FROM (SELECT emp_no, MAX(salary) AS salary
      FROM salaries
      GROUP BY emp_no) s
WHERE e.emp_no = s.emp_no;


CREATE TABLE employees3 (
  emp_no int,
  birth_date date,
  first_name varchar(14),
  last_name varchar(16),
  gender character(1),
  hire_date date,
  dept_no varchar(5),
  from_date date,
  salary int
) PARTITION BY RANGE (date_part('year', hire_date), salary);

SELECT date_part('year', hire_date) AS year, COUNT(*) from employees GROUP BY year ORDER BY year;
SELECT salary, COUNT(*) from employees GROUP BY salary ORDER BY salary;
select * from employees;

select COUNT(*) from employees;

CREATE TABLE employees3_p1 PARTITION OF employees3 FOR VALUES FROM (MINVALUE, MINVALUE) TO (1990, 60001);
CREATE TABLE employees3_p2 PARTITION OF employees3 FOR VALUES FROM (1990, 60001) TO (1995, 100001);
CREATE TABLE employees3_p3 PARTITION OF employees3 FOR VALUES FROM (1995, 100001) TO (MAXVALUE, MAXVALUE);

INSERT INTO employees3 (emp_no, birth_date, first_name, last_name, gender, hire_date, dept_no, from_date, salary)
SELECT emp_no, birth_date, first_name, last_name, gender, hire_date, dept_no, from_date, salary
FROM employees;

-- CONSULTA 1
-- En la tabla employees
EXPLAIN ANALYZE
SELECT emp_no, first_name, last_name, hire_date, salary
FROM employees
WHERE date_part('year', hire_date) BETWEEN 1990 AND 1994
  AND salary BETWEEN 60001 AND 100000;

-- En la tabla employees3
EXPLAIN ANALYZE
SELECT emp_no, first_name, last_name, hire_date, salary
FROM employees3
WHERE date_part('year', hire_date) BETWEEN 1990 AND 1994
  AND salary BETWEEN 60001 AND 100000;

-- CONSULTA 2
-- En la tabla employees
EXPLAIN ANALYZE
SELECT emp_no, first_name, last_name, hire_date, salary
FROM employees
WHERE date_part('year', hire_date) > 1995
  AND salary > 100000;

-- En la tabla employees3
EXPLAIN ANALYZE
SELECT emp_no, first_name, last_name, hire_date, salary
FROM employees3
WHERE date_part('year', hire_date) > 1995
  AND salary > 100000;


-- CONSULTA 3
-- En la tabla employees
EXPLAIN ANALYZE
SELECT emp_no, first_name, last_name, hire_date, salary
FROM employees
WHERE date_part('year', hire_date) < 1990
  AND salary < 60001;

-- En la tabla employees3
EXPLAIN ANALYZE
SELECT emp_no, first_name, last_name, hire_date, salary
FROM employees3
WHERE date_part('year', hire_date) < 1990
  AND salary < 60001;


