#include <cstdio>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <cstring>
#define MAX_B 3
using namespace std;

template <typename T>

int hashFunction(T key, int N)
{
    hash<T> hash_fn;
    int hashedValue = hash_fn(key) % N;
    return hashedValue;
}

struct Record
{
    char nombre[30];
    char carrera[20];
    int ciclo;

    void setData(ifstream &file)
    {
        file.getline(nombre, 30, ',');
        file.getline(carrera, 30, ',');
        file >> ciclo;
        file.get();
    }

    void showData()
    {
        cout << setw(10) << left << nombre;
        cout << setw(10) << left << carrera;
        cout << setw(10) << left << ciclo << endl;
    }

    friend bool operator==(const Record &r1, const Record &r2)
    {
        return strcmp(r1.nombre, r2.nombre) == 0 && strcmp(r1.carrera, r2.carrera) == 0 && r1.ciclo == r2.ciclo;
    }
    friend bool operator!=(const Record &r1, const Record &r2)
    {
        return !(r1 == r2);
    }
};

struct Bucket
{
    Record records[MAX_B];
    int size;
    long next_bucket;

    Bucket()
    {
        size = 0;
        next_bucket = -1;
    }

    void showData()
    {
        cout << "[" << endl;
        for (int i = 0; i < size; i++)
            records[i].showData();
        cout << "]" << endl;
    }
};

class StaticHashFile
{
private:
    string filename = "";
    int numberBuckets = 0;
    int numberRecords = 0;
    int counterBuckets = 0;
    void writeBucket(Bucket bucket, int pos)
    {
        fstream file(filename, ios::in | ios::out | ios::binary);
        file.seekp(pos * sizeof(Bucket));
        file.write((char *)&bucket, sizeof(Bucket));
        file.close();
    }
    void readBucket(Bucket &bucket, int pos)
    {
        fstream file(filename, ios::in | ios::out | ios::binary);
        file.seekg(pos * sizeof(Bucket));
        file.read((char *)&bucket, sizeof(Bucket));
        file.close();
    }

public:
    static void clearFile(string filename)
    {
        fstream file(filename, ios::out | ios::binary);
        file.close();
    }
    StaticHashFile(string _filename, int _N)
    {
        filename = _filename;
        numberBuckets = _N;
        fstream file(filename, ios::in | ios::out | ios::binary);
        // Verificar si el archivo existe, sino crearlo y escribir los buckets
        if (!file.is_open())
        {
            file.open(filename, ios::out | ios::binary);
            Bucket bucket;
            for (int i = 0; i < numberBuckets; i++)
                file.write((char *)&bucket, sizeof(Bucket));
            file.close();
        }
        else
        {
            file.seekg(0, ios::end);
            numberRecords = file.tellg() / sizeof(Bucket);
            file.close();
        }

        file.close();
    }
    void writeRecord(Record record)
    {
        // 1- usando una funcion hashing ubicar la pagina de datos
        // 2- leer la pagina de datos,
        // 3- verificar si size < MAX_B, si es asi, se inserta en esa pagina y se regresa al archivo
        // 4- caso contrario, crear nuevo bucket, insertar ahi, mantener el enlace
        int pos = hashFunction(record.nombre, numberBuckets);
        fstream file(filename, ios::in | ios::out | ios::binary);
        Bucket bucket;
        readBucket(bucket, pos);

        if (bucket.size < MAX_B)
        {
            bucket.records[bucket.size] = record;
            bucket.size++;
            writeBucket(bucket, pos);
        }
        else
        {
            Bucket newBucket;
            newBucket.records[0] = record;
            newBucket.size++;
            newBucket.next_bucket = bucket.next_bucket;
            bucket.next_bucket = numberRecords;
            writeBucket(bucket, pos);
            writeBucket(newBucket, numberRecords);
            numberRecords++;
        }
    }

    Record search(string nombre)
    {
        // 1- usando una funcion hashing ubicar la pagina de datos
        // 2- leer la pagina de datos, ubicar el registro que coincida con el nombre
        // 3- si no se encuentra el registro en esa pagina, ir a la pagina enlazada iterativamente

        int pos = hashFunction(nombre, numberBuckets);
        Bucket bucket;
        readBucket(bucket, pos);
        for (int i = 0; i < bucket.size; i++)
        {
            if (strcmp(bucket.records[i].nombre, nombre.c_str()) == 0)
                return bucket.records[i];
        }

        while (bucket.next_bucket != -1)
        {
            readBucket(bucket, bucket.next_bucket);
            for (int i = 0; i < bucket.size; i++)
            {
                if (strcmp(bucket.records[i].nombre, nombre.c_str()) == 0)
                    return bucket.records[i];
            }
        }

        return Record{};
    }

    void scanAll()
    {
        for (int i = 0; i < numberBuckets; i++)
        {
            Bucket bucket;
            readBucket(bucket, i);
            bucket.showData();
            while (bucket.next_bucket != -1)
            {
                readBucket(bucket, bucket.next_bucket);
                bucket.showData();
            }
        }
    }
};
