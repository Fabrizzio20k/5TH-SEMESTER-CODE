#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct Alumno
{
    char codigo[5];
    char nombre[11];
    char apellidos[20];
    char carrera[20];
    int ciclo;
    float mensualidad;
};

ostream &operator<<(ostream &stream, Alumno &record)
{
    stream.write(record.codigo, 5);
    stream.write(record.nombre, 11);
    stream.write(record.apellidos, 20);
    stream.write(record.carrera, 20);
    stream.write((char *)&record.ciclo, sizeof(int));
    stream.write((char *)&record.mensualidad, sizeof(float));
    stream << flush;
    return stream;
}
istream &operator>>(istream &stream, Alumno &record)
{
    stream.read(record.codigo, 5);
    stream.read(record.nombre, 11);
    stream.read(record.apellidos, 20);
    stream.read(record.carrera, 20);
    stream.read((char *)&record.ciclo, sizeof(int));
    stream.read((char *)&record.mensualidad, sizeof(float));
    return stream;
}

enum DeleteType
{
    MOVE_TO_LAST,
    FREE_LIST
};

class FixedRecord
{
private:
    string filename;
    DeleteType deleteType;

public:
    // Constructor
    FixedRecord(string filename, DeleteType deleteType = MOVE_TO_LAST)
    {
        this->filename = filename;
        this->deleteType = deleteType;

        vector<Alumno> alumnos = this->load();

        ofstream file(this->filename, ios::out | ios::binary);
        if (!file.is_open())
        {
            cerr << "Error al abrir el archivo" << endl;
            exit(1);
        }

        int records = alumnos.size();

        if (deleteType == FREE_LIST)
        {
            int head = -1, idx = -2;
            file.write((char *)&head, sizeof(Alumno) + sizeof(int));

            for (int i = 0; i < alumnos.size(); i++)
            {
                file.write((char *)&alumnos[i], sizeof(Alumno));
                file.write((char *)&idx, sizeof(int));
                file << flush;
            }
        }
        else
        {
            file.write((char *)&records, sizeof(Alumno));

            for (int i = 0; i < alumnos.size(); i++)
            {
                file << alumnos[i];
            }
        }

        file.close();
    }

    // Static methods
    static void writeMockData(string filename = "data.dat")
    {
        Alumno alumnos[] = {
            {"0001", "Juan", "Perez", "Sistemas", 1, 200.45},
            {"0002", "Maria", "Gomez", "Industrial", 2, 250.78},
            {"0003", "Pedro", "Torres", "Sistemas", 3, 300.98},
            {"0004", "Ana", "Lopez", "Industrial", 4, 350.74},
            {"0005", "Luis", "Garcia", "Sistemas", 5, 400.36},
            {"0006", "Rosa", "Mendoza", "Industrial", 6, 450.87},
            {"0007", "Carlos", "Vargas", "Sistemas", 7, 500.97},
            {"0008", "Sofia", "Carranza", "Industrial", 8, 550.75},
            {"0009", "Jorge", "Paredes", "Sistemas", 9, 600.12},
            {"0010", "Elena", "Cordova", "Industrial", 10, 650.37},
            {"0011", "Elena", "Cordova", "Industrial", 7, 950.37},
        };
        ofstream file(filename, ios::out | ios::binary);
        if (!file.is_open())
        {
            cerr << "Error al abrir el archivo" << endl;
            exit(1);
        }

        for (int i = 0; i < 10; i++)
        {
            file << alumnos[i];
        }
        file.close();
    }

    // Methods
    void printData()
    {
        ifstream file(this->filename, ios::in | ios::binary);
        if (!file.is_open())
        {
            cerr << "Error al abrir el archivo" << endl;
            exit(1);
        }
        if (deleteType == MOVE_TO_LAST)
        {
            Alumno alumno;
            file.seekg(sizeof(Alumno), ios::beg);
            for (int i = 0; i < this->countRecords(); i++)
            {
                file >> alumno;
                cout << alumno.codigo << " " << alumno.nombre << " " << alumno.apellidos << " " << alumno.carrera << " " << alumno.ciclo << " " << alumno.mensualidad << endl;
            }
        }
        else
        {
            int next;
            file.read((char *)&next, sizeof(int));
            file.seekg(sizeof(Alumno), ios::cur);
            cout << "Head: " << next << endl;
            Alumno alumno;

            while (file >> alumno)
            {
                file.read((char *)&next, sizeof(int));
                cout << alumno.codigo << " " << alumno.nombre << " " << alumno.apellidos << " " << alumno.carrera << " " << alumno.ciclo << " " << alumno.mensualidad << " " << next << endl;
            }
        }

        file.close();
    }

    int countRecords()
    {
        ifstream file(this->filename, ios::in | ios::binary);
        if (!file.is_open())
        {
            cerr << "Error al abrir el archivo" << endl;
            exit(1);
        }
        int records = 0;

        if (deleteType == MOVE_TO_LAST)
        {
            file.read((char *)&records, sizeof(int));
            file.close();
            return records;
        }

        int idx;

        file.seekg(sizeof(Alumno) + sizeof(int), ios::beg);

        Alumno record;
        while (file >> record)
        {
            file.read((char *)&idx, sizeof(int));
            if (idx == -2)
            {
                records++;
            }
        }

        return records;
    }

    bool deleteRecord(int position)
    {
        fstream file(this->filename, ios::in | ios::out | ios::binary);
        if (!file.is_open())
        {
            cerr << "Error al abrir el archivo" << endl;
            exit(1);
        }

        if (size() == 1)
        {
            return false;
        }

        if (deleteType == MOVE_TO_LAST)
        {
            file.seekg((this->countRecords() * sizeof(Alumno)), ios::beg);
            Alumno lastRecord;
            file >> lastRecord;

            file.seekg((position * sizeof(Alumno)) + sizeof(Alumno), ios::beg);
            Alumno record;
            file >> record;

            file.seekg((position * sizeof(Alumno)) + sizeof(Alumno), ios::beg);
            file << lastRecord;

            file.seekg(0, ios::beg);
            int records = this->countRecords() - 1;
            file.write((char *)&records, sizeof(Alumno));
        }
        else
        {
            int head;
            file.read((char *)&head, sizeof(int));

            // move to the record to delete
            file.seekg((position * (sizeof(Alumno) + sizeof(int))) + (sizeof(Alumno) * 2) + sizeof(int), ios::beg);
            file.write((char *)&head, sizeof(int));

            // update the head
            file.seekg(0, ios::beg);
            file.write((char *)&position, sizeof(int));
        }

        file.close();

        return true;
    }

    vector<Alumno> load()
    {
        vector<Alumno> alumnos;
        ifstream file(this->filename, ios::in | ios::binary);
        if (!file.is_open())
        {
            cerr << "Error al abrir el archivo" << endl;
            exit(1);
        }
        Alumno alumno;
        while (file >> alumno)
        {
            alumnos.push_back(alumno);
        }
        file.close();
        return alumnos;
    }

    void add(Alumno record)
    {
        fstream file(this->filename, ios::in | ios::out | ios::binary);
        if (!file.is_open())
        {
            cerr << "Error al abrir el archivo" << endl;
            exit(1);
        }
        if (deleteType == FREE_LIST)
        {
            int head, temp, index = -2;
            file.read((char *)&head, sizeof(int));

            if (head == -1)
            {
                file.seekg(0, ios::end);
                file << record;
                file.write((char *)&index, sizeof(int));
            }
            else
            {

                file.seekg(0, ios::beg);
                file.seekg(head * (sizeof(Alumno) + sizeof(int)), ios::cur);
                file.seekg(sizeof(Alumno) + sizeof(int) + sizeof(Alumno), ios::cur);
                file.read((char *)&temp, sizeof(int));
                file.seekg(-(sizeof(Alumno) + sizeof(int)), ios::cur);

                file.write((char *)&record, sizeof(Alumno));
                file.write((char *)&index, sizeof(int));

                file.seekg(0, ios::beg);
                file.write((char *)&temp, sizeof(int));
            }
        }
        else
        {

            int records = this->countRecords();

            // change the number of records
            file.seekg(0, ios::beg);
            records++;
            file.write((char *)&records, sizeof(Alumno));

            // get the last register
            file.seekg((records - 1) * sizeof(Alumno) + sizeof(Alumno), ios::beg);
            file << record;
        }
        file.close();
    }

    Alumno readRecord(int pos)
    {
        ifstream file(this->filename, ios::in | ios::binary);
        Alumno record;
        if (deleteType == FREE_LIST)
        {

            int head, counter = 0, idx = 0;
            file.read((char *)&head, sizeof(int));
            file.seekg(sizeof(Alumno) + sizeof(int), ios::beg);
            while (counter < pos)
            {
                file.seekg(sizeof(Alumno) + sizeof(int), ios::cur);
                file.read((char *)&record, sizeof(Alumno));
                file.read((char *)&idx, sizeof(int));
                if (idx == -2)
                {
                    counter++;
                }
            }
        }
        else
        {
            file.seekg((pos * sizeof(Alumno)) + sizeof(int), ios::beg);
            file >> record;
        }
        return record;
    }

    int size()
    {
        ifstream file(filename, ios::in | ios::binary);

        if (!file.is_open())
            throw("No se pudo abrir el archivo");

        file.seekg(0, ios::end);
        long total_bytes = file.tellg();
        file.close();

        if (deleteType == MOVE_TO_LAST)
        {
            return total_bytes / sizeof(Alumno);
        }
        else if (deleteType == FREE_LIST)
        {
            return total_bytes / (sizeof(Alumno) + sizeof(int));
        }

        return 0;
    }
};

void testDeleteRecordsMoveToLast()
{
    FixedRecord::writeMockData();
    FixedRecord fixedRecord("data.dat");

    cout << "------------------------------------" << endl;
    cout << "MOVE_TO_LAST" << endl;
    cout << "------------------------------------" << endl
         << endl;

    cout << "------------------------------------" << endl;
    cout << "Cargando datos iniciales:" << endl;
    cout << "------------------------------------" << endl
         << endl;
    fixedRecord.printData();

    cout << endl
         << "------------------------------------" << endl;
    cout << "Eliminando registros (1, 2, 3):" << endl;
    cout << "------------------------------------" << endl
         << endl;
    fixedRecord.deleteRecord(1);
    fixedRecord.deleteRecord(2);
    fixedRecord.deleteRecord(3);
    fixedRecord.printData();

    cout << "Total records: " << fixedRecord.countRecords() << flush << endl;

    cout << endl
         << "------------------------------------" << endl;
    cout << "Agregando nuevos registros:" << endl;
    cout << "------------------------------------" << endl
         << endl;
    Alumno alumno1 = {"0012", "Elena", "Cordova", "Industrial", 7, 950.37};
    Alumno alumno2 = {"0013", "Elena", "Cordova", "Industrial", 7, 950.37};
    Alumno alumno3 = {"0014", "Piero", "Santiago", "Industrial", 7, 950.37};
    Alumno alumno4 = {"0015", "Piero", "Santiago", "Industrial", 7, 950.37};

    fixedRecord.add(alumno1);
    fixedRecord.add(alumno2);
    fixedRecord.add(alumno3);
    fixedRecord.add(alumno4);
    fixedRecord.printData();
    cout << "Total records: " << fixedRecord.countRecords() << flush << endl;
}

void testDeleteRecordsFreeList()
{
    FixedRecord::writeMockData();
    FixedRecord fixedRecord("data.dat", FREE_LIST);

    cout << endl
         << "------------------------------------" << endl;
    cout << "FREE_LIST" << endl;
    cout << "------------------------------------" << endl
         << endl;

    cout << "------------------------------------" << endl;
    cout << "Cargando datos iniciales:" << endl;
    cout << "------------------------------------" << endl
         << endl;
    fixedRecord.printData();

    cout << endl
         << "------------------------------------" << endl;
    cout << "Eliminando registros (3, 5, 7):" << endl;
    cout << "------------------------------------" << endl
         << endl;
    fixedRecord.deleteRecord(3);
    fixedRecord.deleteRecord(5);
    fixedRecord.deleteRecord(7);
    fixedRecord.printData();

    cout << "Total records: " << fixedRecord.countRecords() << flush << endl;

    cout << endl
         << "------------------------------------" << endl;
    cout << "Agregando nuevos registros:" << endl;
    cout << "------------------------------------" << endl
         << endl;
    Alumno alumno1 = {"0012", "Elena", "Cordova", "Industrial", 7, 950.37};
    Alumno alumno2 = {"0013", "Jeffrey", "Monja", "CS", 7, 3950.37};
    Alumno alumno3 = {"0014", "Piero", "Santiago", "Industrial", 7, 950.37};
    Alumno alumno4 = {"0015", "Piero", "Santiago", "Industrial", 7, 950.37};

    fixedRecord.add(alumno1);
    fixedRecord.add(alumno2);
    fixedRecord.add(alumno3);
    fixedRecord.add(alumno4);
    fixedRecord.printData();
    cout << "Total records: " << fixedRecord.countRecords() << flush << endl;
}

int main()
{
    testDeleteRecordsMoveToLast();
    testDeleteRecordsFreeList();

    return 0;
}