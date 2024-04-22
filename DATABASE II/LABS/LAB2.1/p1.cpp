#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <algorithm>

using namespace std;

struct Registro
{
    char codigo[5];
    char nombre[20];
    char carrera[15];
    int ciclo;
    Registro() = default;
    Registro(string codigo, string nombre, string carrera, int ciclo)
    {
        strcpy(this->codigo, codigo.c_str());
        strcpy(this->nombre, nombre.c_str());
        strcpy(this->carrera, carrera.c_str());
        this->ciclo = ciclo;
    }
    void showData()
    {
        cout << codigo << " | " << nombre << " | " << carrera << " | " << ciclo << endl;
    }
};

ostream &operator<<(ostream &file, const Registro &r)
{
    file.write(r.codigo, 5);
    file.write(r.nombre, 20);
    file.write(r.carrera, 15);
    file.write((char *)&r.ciclo, sizeof(int));
    return file;
}

istream &operator>>(istream &file, Registro &r)
{
    file.read(r.codigo, 5);
    file.read(r.nombre, 20);
    file.read(r.carrera, 15);
    file.read((char *)&r.ciclo, sizeof(int));
    return file;
}

class SequentialFile
{
private:
    string filename;
    int maxRegistros;
    vector<Registro> getRegistros(string filename_)
    {
        vector<Registro> registros;
        ifstream file(filename_, ios::binary);
        Registro r;
        while (file >> r)
        {
            registros.push_back(r);
        }
        file.close();
        return registros;
    }

public:
    static void cleanFiles(string filename, string aux)
    {
        ofstream file(filename, ios::binary);
        file.close();
        file.open(aux, ios::binary);
        file.close();
    }

    SequentialFile(string filename, int maxRegistros = 5)
    {
        this->filename = filename;
        this->maxRegistros = maxRegistros;
        ofstream file("aux.dat", ios::binary);
        file.close();
    }
    void insertAll(vector<Registro> registros)
    {
        sort(registros.begin(), registros.end(), [](Registro &a, Registro &b)
             { return strcmp(a.codigo, b.codigo) < 0; });
        ofstream file(filename, ios::binary);
        for (auto &r : registros)
        {
            file << r;
        }
        file.close();
    }
    Registro search(string codigo)
    {
        Registro r;
        ifstream file(filename, ios::binary);
        while (file >> r)
        {
            if (strcmp(r.codigo, codigo.c_str()) == 0)
            {
                file.close();
                return r;
            }
        }
        file.close();
        return Registro();
    }
    vector<Registro> rangeSearch(string start, string end)
    {
        vector<Registro> registros;
        ifstream file(filename, ios::binary);
        Registro r;
        while (file >> r)
        {
            if (strcmp(r.codigo, start.c_str()) >= 0 && strcmp(r.codigo, end.c_str()) <= 0)
            {
                registros.push_back(r);
            }
        }
        file.close();
        return registros;
    }
    void add(Registro r)
    {
        vector<Registro> registrosAux = getRegistros("aux.dat");
        if (registrosAux.size() >= maxRegistros)
        {
            vector<Registro> registros = getRegistros(filename);
            registros.insert(registros.end(), registrosAux.begin(), registrosAux.end());
            insertAll(registros);
            cleanAux();
        }
        ofstream file("aux.dat", ios::binary | ios::app);
        file << r;
        file.close();
    }
    void printAll()
    {
        vector<Registro> registros = getRegistros(filename);
        for (auto &r : registros)
        {
            r.showData();
        }
    }
    void cleanAux()
    {
        ofstream file("aux.dat", ios::binary);
        file.close();
    }
    ~SequentialFile()
    {
        vector<Registro> registrosAux = getRegistros("aux.dat");
        if (registrosAux.size() > 0)
        {
            vector<Registro> registros = getRegistros(filename);
            registros.insert(registros.end(), registrosAux.begin(), registrosAux.end());
            insertAll(registros);
            cleanAux();
        }
    }
};

void writeData(string filename)
{
    SequentialFile::cleanFiles(filename, "aux.dat");
    SequentialFile sf(filename, 5);
    vector<Registro> registros = {
        Registro("A01", "Juan Perez", "Ing. Sistemas", 5),
        Registro("A09", "Maria Lopez", "Ing. Industrial", 6),
        Registro("A08", "Pedro Rodriguez", "Ing. Civil", 7),
        Registro("B06", "Ana Torres", "Ing. Ambiental", 8),
        Registro("I09", "Luisa Garcia", "Ing. Mecanica", 9),
        Registro("I14", "Carlos Ramirez", "Ing. Sistemas", 10),
        Registro("I02", "Josefa Paredes", "Ing. Industrial", 11),
        Registro("B02", "Rosa Melgar", "Ing. Civil", 12),
        Registro("B04", "Manuel Paredes", "Ing. Ambiental", 13),
        Registro("I04", "Brigith Hinojosa", "Ing. Mecanica", 14),
    };

    for (auto &r : registros)
    {
        sf.add(r);
    }
}
void readData(string filename)
{
    SequentialFile sf(filename, 5);
    cout << "------------All data------------" << endl;
    sf.printAll();
    cout << "------------Search A09------------" << endl;
    Registro r = sf.search("A09");
    r.showData();
    cout << "------------Range search A08 to I04------------" << endl;
    vector<Registro> registros = sf.rangeSearch("A08", "I04");
    for (auto &r : registros)
    {
        r.showData();
    }
}

int main()
{
    SequentialFile::cleanFiles("data.dat", "aux.dat");
    writeData("data.dat");
    readData("data.dat");
    return 0;
}