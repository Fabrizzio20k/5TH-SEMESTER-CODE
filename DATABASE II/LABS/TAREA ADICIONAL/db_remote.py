import psycopg2
from faker import Faker
import random
from datetime import date

# Configuración de la conexión a la base de datos
conn = psycopg2.connect(
    host="localhost",
    port=5435,  # Puerto específico para db3
    database="db3",
    user="user3",
    password="password3"
)

cursor = conn.cursor()

fake = Faker('es_ES')

ventas = []
reclamos = []

# Códigos de locales específicos para este fragmento
cod_locales = [f"LOC{str(i).zfill(2)}" for i in range(9, 13)]
start_date = date(2017, 1, 1)
end_date = date(2024, 12, 31)
estados = ['Pendiente', 'En Proceso', 'Cerrado']

# Generar datos aleatorios con id incremental
for i in range(1000):
    dni_cliente = str(random.randint(70000000, 79999999))
    fecha_venta = fake.date_between(start_date=start_date, end_date=end_date)
    cod_local = random.choice(cod_locales)
    importe_total = round(random.uniform(10.0, 1000.0), 2)
    id_empleado = f"EMP{
        str(fake.random_number(digits=3, fix_len=True)).zfill(3)}"

    ventas.append((i + 1, dni_cliente, fecha_venta,
                  cod_local, importe_total, id_empleado))

    dni_cliente = str(random.randint(70000000, 79999999))
    fecha_reclamo = fake.date_between(start_date=start_date, end_date=end_date)
    cod_local = random.choice(cod_locales)
    descripcion = fake.text(max_nb_chars=100)
    estado = random.choice(estados)

    reclamos.append((i + 1, dni_cliente, fecha_reclamo,
                    cod_local, descripcion, estado))

# Insertar las tuplas en la base de datos
insert_query_venta = """
INSERT INTO venta_2017_2024 (IdVenta, DNI_Cliente, FechaVenta, CodLocal, ImporteTotal, IdEmpleado)
VALUES (%s, %s, %s, %s, %s, %s)
"""

insert_query_reclamo = """
INSERT INTO reclamo_loc9_loc12 (IdReclamo, DNI_Cliente, FechaReclamo, CodLocal, Descripcion, Estado)
VALUES (%s, %s, %s, %s, %s, %s)
"""

try:
    cursor.executemany(insert_query_venta, ventas)
    cursor.executemany(insert_query_reclamo, reclamos)
    conn.commit()
    print("Datos insertados correctamente en las tablas venta_2017_2024 y reclamo_loc9_loc12")
except Exception as e:
    print(f"Error al insertar los datos: {e}")
    conn.rollback()

# Cerrar la conexión
cursor.close()
conn.close()
