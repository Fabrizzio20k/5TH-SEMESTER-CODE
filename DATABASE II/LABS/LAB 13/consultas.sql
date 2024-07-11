/*P1*/

SELECT CARGO FROM EMP
WHERE EMP.NOMBRE = " Joe "
OR ( 
    NOT ( EMP.CARGO = "Programador" )
    AND ( EMP.CARGO = "Programador" OR EMP.CARGO = "Analista" )
    AND NOT ( EMP.CARGO = "Analista" ) 
)

-- CONSULTA OPTIMIZADA

SELECT CARGO FROM EMP
WHERE EMP.NOMBRE = " Joe "
OR ( 
    EMP.CARGO = "Programador" 
    AND EMP.CARGO = "Analista" 
)