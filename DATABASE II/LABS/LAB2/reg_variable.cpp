#include <cstring>
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>

using namespace std;

struct Alumno
{
    string nombre;  // 8
    string carrera; // 8
    int edad;       // 4
    float talla;    // 4

    void write(ofstream &file)
    {
        file.write((char *)&edad, sizeof(edad));
        file.write((char *)&talla, sizeof(talla));
        writeString(file, nombre);
        writeString(file, carrera);
    }

    void writeString(ofstream &file, string cadena)
    {
        int size = cadena.size();
        file.write((char *)&size, sizeof(int));
        file.write(cadena.c_str(), size);
    }

    void read(ifstream &file)
    {
        file.read((char *)&edad, sizeof(edad));
        file.read((char *)&talla, sizeof(talla));
        readString(file, nombre);
        readString(file, carrera);
    }

    void readString(ifstream &file, string &cadena)
    {
        int size = 0;
        file.read((char *)&size, sizeof(int));
        char *buffer = new char[size];
        file.read(buffer, size);
        cadena = buffer;
    }

    int size_of()
    {
        return sizeof(edad) + sizeof(talla) + nombre.size() + carrera.size();
    }
};
