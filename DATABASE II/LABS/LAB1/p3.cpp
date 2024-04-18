#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

struct Matricula
{
    string codigo;
    int ciclo;
    float mensualidad;
    string observaciones;

    int size()
    {
        return sizeof(int) + codigo.size() + sizeof(int) + sizeof(float) + sizeof(int) + observaciones.size();
    }
};

ostream &operator<<(ostream &stream, Matricula &record)
{
    int size;

    size = record.codigo.size();
    stream.write((char *)&size, sizeof(int));
    stream.write(record.codigo.c_str(), size);

    stream.write((char *)&record.ciclo, sizeof(int));
    stream.write((char *)&record.mensualidad, sizeof(float));

    size = record.observaciones.size();
    stream.write((char *)&size, sizeof(int));
    stream.write(record.observaciones.c_str(), size);

    stream << flush;
    return stream;
}

istream &operator>>(istream &stream, Matricula &record)
{
    int size;

    stream.read((char *)&size, sizeof(int));
    char *buffer = new char[size];
    stream.read(buffer, size);
    record.codigo = string(buffer, size);

    stream.read((char *)&record.ciclo, sizeof(int));
    stream.read((char *)&record.mensualidad, sizeof(float));

    stream.read((char *)&size, sizeof(int));
    buffer = new char[size];
    stream.read(buffer, size);
    record.observaciones = string(buffer, size);

    return stream;
}

class NumberRecord
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

        Matricula matriculas[5] = {
            {"001", 1, 1000.45, "Al dia"},
            {"002", 2, 800.34, "Pago pendiente"},
            {"003", 3, 567.43, "Error en el pago"},
            {"004", 4, 764.21, "Al dia"},
            {"005", 5, 943.79, "Pago pendiente"}};

        for (int i = 0; i < 5; i++)
        {
            file << matriculas[i];
        }

        file.close();

        // Creating metadata file
        ofstream metadata("metadata.dat", ios::out | ios::binary);
        if (!metadata.is_open())
        {
            cerr << "Error opening metadata file" << endl;
            return;
        }

        int pos = 0;
        for (int i = 0; i < 5; i++)
        {
            metadata.write((char *)&pos, sizeof(pos));
            pos += matriculas[i].size();
            metadata.write((char *)&pos, sizeof(pos));
        }
    }

    // Constructor
    NumberRecord(string filename)
    {
        this->filename = filename;
    }

    vector<Matricula> load()
    {
        vector<Matricula> records;
        ifstream file(this->filename, ios::in | ios::binary);
        ifstream metadata("metadata.dat", ios::in | ios::binary);
        if (!file.is_open() || !metadata.is_open())
        {
            cerr << "Error opening file" << endl;
            return records;
        }

        // Getting metadata
        int start, end;
        while (true)
        {
            metadata.read((char *)&start, sizeof(start));
            metadata.read((char *)&end, sizeof(end));
            if (metadata.eof())
                break;

            file.seekg(start, ios::beg);
            Matricula record;
            file >> record;
            records.push_back(record);
        }
        return records;
    }
    void add(Matricula record)
    {
        ofstream file(this->filename, ios::out | ios::app | ios::binary);
        if (!file.is_open())
        {
            cerr << "Error opening file" << endl;
            return;
        }
        file << record;
        file.close();

        // Updating metadata
        fstream metadata("metadata.dat", ios::in | ios::out | ios::app | ios::binary);
        if (!metadata.is_open())
        {
            cerr << "Error opening metadata file" << endl;
            return;
        }

        int pos = 0;
        metadata.seekg(-sizeof(int), ios::end);
        metadata.read((char *)&pos, sizeof(pos));
        metadata.seekp(0, ios::end);
        metadata.write((char *)&pos, sizeof(pos));
        pos += record.size();
        metadata.write((char *)&pos, sizeof(pos));

        metadata.close();
    }
    Matricula readRecord(int pos)
    {
        ifstream file(this->filename, ios::in | ios::binary);
        ifstream metadata("metadata.dat", ios::in | ios::binary);
        if (!file.is_open() || !metadata.is_open())
        {
            cerr << "Error opening file" << endl;
            return Matricula();
        }

        // Getting metadata
        int start, end;
        metadata.seekg(pos * 2 * sizeof(int), ios::beg);
        metadata.read((char *)&start, sizeof(start));
        metadata.read((char *)&end, sizeof(end));

        file.seekg(start, ios::beg);
        Matricula record;
        file >> record;
        return record;
    }
};

void test()
{
    NumberRecord record("matriculas.dat");
    vector<Matricula> records = record.load();

    cout << "------------------------------------" << endl;
    cout << "Cargando datos iniciales" << endl;
    cout << "------------------------------------" << endl
         << endl;

    for (int i = 0; i < records.size(); i++)
    {
        cout << i + 1 << ": " << records[i].codigo << " " << records[i].ciclo << " " << records[i].mensualidad << " " << records[i].observaciones << endl;
    }

    Matricula newRecord = {"006", 6, 1234.56, "Al dia"};
    Matricula newRecord2 = {"007", 7, 1234.56, "Al dia"};
    Matricula newRecord3 = {"008", 8, 1234.56, "Pendiente"};
    Matricula newRecord4 = {"009", 9, 1234.56, "Error"};
    Matricula newRecord5 = {"0010", 10, 1234.56, "No pago"};
    record.add(newRecord);
    record.add(newRecord2);
    record.add(newRecord3);
    record.add(newRecord4);
    record.add(newRecord5);

    records = record.load();

    cout << "------------------------------------" << endl;
    cout << "Cargando datos despues de agregar" << endl;
    cout << "------------------------------------" << endl;

    for (int i = 0; i < records.size(); i++)
    {
        cout << i + 1 << ": " << records[i].codigo << " " << records[i].ciclo << " " << records[i].mensualidad << " " << records[i].observaciones << endl;
    }

    Matricula record2 = record.readRecord(2);
    cout << "------------------------------------" << endl;
    cout << "Leyendo registro en la posicion 2 (partiendo de 0)" << endl;
    cout << "------------------------------------" << endl;
    cout << record2.codigo << " " << record2.ciclo << " " << record2.mensualidad << " " << record2.observaciones << endl;

    Matricula record3 = record.readRecord(7);
    cout << "------------------------------------" << endl;
    cout << "Leyendo registro en la posicion 7 (partiendo de 0)" << endl;
    cout << "------------------------------------" << endl;
    cout << record3.codigo << " " << record3.ciclo << " " << record3.mensualidad << " " << record3.observaciones << endl;
}

int main()
{
    NumberRecord::writeMockData("matriculas.dat");
    test();
    return 0;
}