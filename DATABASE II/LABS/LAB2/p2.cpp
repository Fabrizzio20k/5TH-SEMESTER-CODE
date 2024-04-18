#include <iostream>
#include <fstream>
#include <cstdio>
#include <map>
using namespace std;

struct Record
{
    char nombre[30];
    char carrera[20];
    int ciclo;

    void setData()
    {
        cout << "Alumno:";
        cin >> nombre;
        cout << "Carrera: ";
        cin >> carrera;
        cout << "Ciclo: ";
        cin >> ciclo;
    }

    void showData()
    {
        cout << "\nNombre: " << nombre;
        cout << "\nCarrera: " << carrera;
        cout << "\nCiclo : " << ciclo;
    }

    string getKey()
    {
        return nombre;
    }
};

class RandomFile
{
private:
    string fileName;
    string indexName;

public:
    map<string, long> index;
    RandomFile(string _fileName)
    {
        this->fileName = _fileName;
        this->indexName = _fileName + "_ind";
        readIndex();
    }

    ~RandomFile()
    {
        writeIndex();
    }

    void readIndex()
    {
        ifstream indexFile(indexName, ios::binary);
        char key[30];
        long pos;
        while (indexFile.read((char *)&key, 30) && indexFile.read((char *)&pos, sizeof(pos)))
        {
            index[key] = pos;
        }
        indexFile.close();
    }

    void writeIndex()
    {
        ofstream indexFile(indexName, ios::binary);
        for (auto it = index.begin(); it != index.end(); it++)
        {
            // Write the key
            string key = it->first;
            indexFile.write(key.c_str(), key.size());

            // Make sure to write 30 bytes
            for (int i = key.size(); i < 30; i++)
                indexFile.put(0);

            indexFile.write((char *)&it->second, sizeof(it->second));
        }
        indexFile.close();
    }

    void write_record(Record record)
    {
        ofstream dataFile(fileName, ios::binary | ios::ate | ios::app);
        long pos = dataFile.tellp();
        dataFile.write((char *)&record, sizeof(record));
        dataFile.close();
        index[record.getKey()] = pos;

        ofstream indexFile(indexName, ios::binary | ios::app);
        indexFile.write((char *)&record.nombre, 30);
        indexFile.write((char *)&pos, sizeof(pos));

        indexFile.close();
    }

    Record *search(string key)
    {
        if (index.find(key) == index.end())
        {
            return nullptr;
        }
        ifstream dataFile(fileName, ios::binary);
        dataFile.seekg(index[key]);
        Record *record = new Record;
        dataFile.read((char *)record, sizeof(*record));
        dataFile.close();
        return record;
    }

    void scanAll()
    {
        ifstream dataFile(fileName, ios::binary);
        Record record;
        while (dataFile.read((char *)&record, sizeof(record)))
        {
            record.showData();
        }
        dataFile.close();
    }

    void scanAllByIndex()
    {
        for (auto &pair : index)
        {
            Record *record = search(pair.first);
            record->showData();
            delete record;
        }
    }
};

void writeFile(string filename)
{

    ofstream ofs;
    ofs.open(filename, ofstream::out | ofstream::trunc);
    ofs.close();

    ofs.open(filename + "_ind", ofstream::out | ofstream::trunc);
    ofs.close();

    RandomFile file(filename);
    Record r1 = {"Juan", "Ing. Sistemas", 5};
    Record r2 = {"Pedro", "Ing. Industrial", 6};
    Record r3 = {"Maria", "Ing. Civil", 7};
    Record r4 = {"Jose", "Ing. Mecanica", 8};

    file.write_record(r1);
    file.write_record(r2);
    file.write_record(r3);
    file.write_record(r4);

    /*
    for (int i = 0; i < 4; i++)
    {
        record.setData();
        file.write_record(record);
    }
    */
}

void readFile(string filename)
{
    RandomFile file(filename);
    cout << "--------- show all data -----------\n";
    file.scanAll();
    cout << "\n\n--------- show all sorted data -----------\n";
    file.scanAllByIndex();
}

int main()
{
    writeFile("data2.dat");
    readFile("data2.dat");
    return 0;
}