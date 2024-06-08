import csv
import requests
import json
import math
from abc import ABC, abstractmethod


class Coordenada:
    def __init__(self, latitud, longitud):
        self.latitud = latitud
        self.longitud = longitud


class Ciudad:
    def __init__(self, nombre_pais, nombre_ciudad):
        self.nombre_pais = nombre_pais
        self.nombre_ciudad = nombre_ciudad


class IObtenerCoordenadas(ABC):
    @abstractmethod
    def obtener_coordenadas(self, ciudad):
        pass


class ObtenerCoordenadasCSV(IObtenerCoordenadas):
    def __init__(self, archivo_csv):
        self.archivo_csv = archivo_csv

    def obtener_coordenadas(self, ciudad):
        with open(self.archivo_csv, mode='r') as file:
            reader = csv.DictReader(file)
            for row in reader:
                if row['city'].lower() == ciudad.nombre_ciudad.lower() and row['country'].lower() == ciudad.nombre_pais.lower():
                    return Coordenada(float(row['lat']), float(row['lng']))
        return None


class ObtenerCoordenadasAPI(IObtenerCoordenadas):
    def obtener_coordenadas(self, ciudad):
        url = f'''https://nominatim.openstreetmap.org/search?q={
            ciudad.nombre_ciudad},{ciudad.nombre_pais}&format=json'''
        data = requests.get(url)
        data = data.json()
        if data:
            latitud = float(data[0]['lat'])
            longitud = float(data[0]['lon'])
            return Coordenada(latitud, longitud)
        return None


class ObtenerCoordenadasMock(IObtenerCoordenadas):
    def obtener_coordenadas(self, ciudad):
        return Coordenada(0.0, 0.0)


def calcular_distancia_haversine(coord1, coord2):
    R = 6371.0  # Radio de la Tierra en kilómetros

    lat1, lon1 = math.radians(coord1.latitud), math.radians(coord1.longitud)
    lat2, lon2 = math.radians(coord2.latitud), math.radians(coord2.longitud)

    dlat = lat2 - lat1
    dlon = lon2 - lon1

    a = math.sin(dlat / 2)**2 + math.cos(lat1) * \
        math.cos(lat2) * math.sin(dlon / 2)**2
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))

    distancia = R * c
    return distancia


def obtener_coordenadas(ciudad, metodo):
    return metodo.obtener_coordenadas(ciudad)


def main():
    ciudad1 = Ciudad("Peru", "Lima")
    ciudad2 = Ciudad("Argentina", "Buenos Aires")

    metodo_csv = ObtenerCoordenadasCSV('worldcities.csv')
    metodo_api = ObtenerCoordenadasAPI()
    metodo_mock = ObtenerCoordenadasMock()

    metodo = metodo_api

    coord1 = obtener_coordenadas(ciudad1, metodo)
    coord2 = obtener_coordenadas(ciudad2, metodo)

    if coord1 and coord2:
        distancia = calcular_distancia_haversine(coord1, coord2)
        print(f'''La distancia entre {ciudad1.nombre_ciudad}, {ciudad1.nombre_pais} y {
              ciudad2.nombre_ciudad}, {ciudad2.nombre_pais} es {distancia:.2f} km.''')
    else:
        print("No se pudieron obtener las coordenadas de una o ambas ciudades.")


if __name__ == "__main__":
    main()
