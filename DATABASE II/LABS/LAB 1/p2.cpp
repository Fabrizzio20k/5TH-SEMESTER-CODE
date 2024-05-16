#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

struct Alumno
{
    string nombre;
    string apellidos;
    string carrera;
    float mensualidad;
};

ostream &operator<<(ostream &stream, Alumno &record)
{
    stream << record.nombre << "|";
    stream << record.apellidos << "|";
    stream << record.carrera << "|";
    stream << record.mensualidad << "\n";
    stream << flush;
    return stream;
}
istream &operator>>(istream &stream, Alumno &record)
{
    getline(stream, record.nombre, '|');
    getline(stream, record.apellidos, '|');
    getline(stream, record.carrera, '|');
    stream >> record.mensualidad;
    stream.ignore();
    return stream;
}

class VariableRecord
{
private:
    string filename;

public:
    // Static methods
    static void writeMockData(string filename)
    {
        ofstream file(filename, ios::out | ios::binary);
        if (!file.is_open())
        {
            cerr << "Error opening file" << endl;
            return;
        }

        Alumno alumnos[5] = {
            {"Juan", "Perez", "CS", 1000.45},
            {"Maria", "Gomez", "Civil", 800.34},
            {"Pedro", "Lopez", "Ingenieria", 567.43},
            {"Ana", "Torres", "Ingenieria", 764.21},
            {"Luis", "Garcia", "Ingenieria", 943.79}};

        for (int i = 0; i < 5; i++)
        {
            file << alumnos[i];
        }

        file.close();
    }

    // Constructor
    VariableRecord(string filename)
    {
        this->filename = filename;
    }
    vector<Alumno> load()
    {
        vector<Alumno> records;
        ifstream file(this->filename, ios::in | ios::binary);
        if (!file.is_open())
        {
            cerr << "Error opening file" << endl;
            return records;
        }

        while (true)
        {
            Alumno record;
            file >> record;
            if (file.eof())
                break;
            records.push_back(record);
        }
        return records;
    }
    void add(Alumno record)
    {
        ofstream file(this->filename, ios::out | ios::app | ios::binary);
        if (!file.is_open())
        {
            cerr << "Error opening file" << endl;
            return;
        }
        file << record;
        file.close();
    }
    Alumno readRecord(int pos)
    {
        ifstream file(this->filename, ios::in | ios::binary);
        if (!file.is_open())
        {
            cerr << "Error opening file" << endl;
            return Alumno();
        }

        Alumno record;
        int currentPos = 0;
        while (currentPos <= pos)
        {
            if (file.eof())
            {
                cerr << "Position out of range" << endl;
                return Alumno();
            }
            file >> record;
            currentPos++;
        }
        return record;
    }
};

void test()
{
    VariableRecord variableRecord("alumnos.txt");
    vector<Alumno> records = variableRecord.load();

    cout << "------------------------------------" << endl;
    cout << "Cargando datos iniciales" << endl;
    cout << "------------------------------------" << endl
         << endl;

    cout << "Total records: " << records.size() << endl
         << endl;
    for (int i = 0; i < records.size(); i++)
    {
        cout << i + 1 << ": " << records[i].nombre << " " << records[i].apellidos << " " << records[i].carrera << " " << records[i].mensualidad << endl;
    }

    cout << "------------------------------------" << endl;
    cout << "Agregando 4 nuevos registros" << endl;
    cout << "------------------------------------" << endl
         << endl;
    Alumno newRecord = {"Carlos", "Gutierrez", "Ingenieria", 1234.56};
    Alumno newRecord2 = {"Brigith", "Hinojosa", "Medicina", 3500.23};
    Alumno newRecord3 = {"Luis", "Garcia", "Ingenieria", 943.79};
    Alumno newRecord4 = {"Jeffrey", "Monja", "CS", 5004.32};
    variableRecord.add(newRecord);
    variableRecord.add(newRecord2);
    variableRecord.add(newRecord3);
    variableRecord.add(newRecord4);
    records = variableRecord.load();
    cout << "Total records: " << records.size() << endl
         << endl;

    for (int i = 0; i < records.size(); i++)
    {
        cout << i + 1 << ": " << records[i].nombre << " " << records[i].apellidos << " " << records[i].carrera << " " << records[i].mensualidad << endl;
    }

    cout << "------------------------------------" << endl;
    cout << "Leyendo registro en posicion 2 (partiendo de 0)" << endl;
    cout << "------------------------------------" << endl;
    Alumno record = variableRecord.readRecord(2);
    cout << record.nombre << " " << record.apellidos << " " << record.carrera << " " << record.mensualidad << endl;
}

int main()
{
    VariableRecord::writeMockData("alumnos.txt");
    test();

    return 0;
}