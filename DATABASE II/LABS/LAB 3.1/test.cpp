//
// Created by iansg on 4/25/2024.
//

#include <cstdio>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <map>
#include <cassert>
using namespace std;

const int N_BUCKETS = 4;
const int BLOCKING_FACTOR = 3;

struct Record
{
    char name[30];
    char career[20];
    int semester;

    void setData(ifstream &file)
    {
        file.getline(name, 30, ',');
        file.getline(career, 30, ',');
        file >> semester;
        file.get();
    }

    void showData()
    {
        cout << setw(30) << left << name;
        cout << setw(20) << left << career;
        cout << setw(3) << left << semester << endl;
    }

    friend bool operator==(const Record &r1, const Record &r2)
    {
        return strcmp(r1.name, r2.name) == 0 && strcmp(r1.career, r2.career) == 0 && r1.semester == r2.semester;
    }
    friend bool operator!=(const Record &r1, const Record &r2)
    {
        return !(r1 == r2);
    }
};

struct Bucket
{
    Record records[BLOCKING_FACTOR]{};
    int size{};
    long nextBucket{-1};

    Bucket() = default;

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
    char *filename{};
    int sz{}; // current number of buckets, can be higher than n
    int n{};  // max number of buckets
    int m{};  // number of buckets per file
    hash<string> fhash{};

    void setNumberOfBuckets()
    {
        ifstream f(filename, ios::binary);
        f.seekg(0, ios::end);
        sz = int(f.tellg() / sizeof(Bucket));
        f.close();
    }

    Bucket readBucket(int pos)
    {
        Bucket bucket{};
        ifstream f(filename, ios::binary);
        f.seekg(pos * (int)sizeof(Bucket));
        f.read(reinterpret_cast<char *>(&bucket), sizeof(Bucket));
        f.close();
        return bucket;
    }

    void writeBucket(Bucket &bucket, int pos)
    {
        ofstream f(filename, ios::in | ios::binary);
        f.seekp(pos * (int)sizeof(Bucket));
        f.write(reinterpret_cast<char *>(&bucket), sizeof(Bucket));
        f.close();
    }

    void writeBuckets()
    {
        Bucket bucket{};
        ofstream f(filename, ios::binary);
        for (int i = 0; i < n; ++i)
            f.write(reinterpret_cast<char *>(&bucket), sizeof(Bucket));
        f.close();
    }

public:
    StaticHashFile(string &filename, int nBuckets)
    {
        this->filename = new char[filename.size()];
        strcpy(this->filename, filename.c_str());
        n = nBuckets;
        m = BLOCKING_FACTOR;
        fstream f(filename, ios::in | ios::out | ios::binary);
        bool exists = f.is_open();
        f.close();
        if (!exists)
        {
            writeBuckets();
            sz = n;
        }
        else
            setNumberOfBuckets();
    }

    void writeRecord(Record record)
    {
        int pos = fhash(string(record.name)) % n; // negative with cast to int

        Bucket bucket = readBucket(pos);
        while (bucket.size == m)
        {
            if (bucket.nextBucket >= 0)
            {
                pos = bucket.nextBucket;
                bucket = readBucket(pos);
            }
            else
            {
                bucket.nextBucket = sz;
                writeBucket(bucket, pos);

                bucket = Bucket{};
                pos = sz++;
            }
        }

        bucket.records[bucket.size++] = record;
        writeBucket(bucket, pos);
    }

    Record search(char *name)
    {
        int pos = fhash(string(name)) % n;
        Bucket bucket = readBucket(pos);

        int i{};
        while (i < bucket.size)
        {
            Record record = bucket.records[i];
            if (strcmp(record.name, name) == 0)
                return record;

            if (++i == bucket.size)
            {
                if (bucket.nextBucket == -1)
                    break;
                bucket = readBucket(bucket.nextBucket);
                i = 0;
            }
        }
        throw runtime_error("No record matches the name: " + string(name) + ".");
    }

    void scanAll()
    {
        for (int i = 0; i < sz; ++i)
        {
            Bucket bucket = readBucket(i);
            for (int j = 0; j < bucket.size; ++j)
                bucket.records[j].showData();
        }
    }
};

void writeFile(string filename)
{
    StaticHashFile file(filename, N_BUCKETS);
    Record record{};
    ifstream fileIn("data.txt");
    while (true)
    {
        record.setData(fileIn);
        if (fileIn.eof())
            break;
        record.showData();
        file.writeRecord(record);
    }
    fileIn.close();
}

void readFile(string filename)
{
    StaticHashFile file(filename, N_BUCKETS);
    cout << "--------- show all data -----------\n";
    file.scanAll();
    cout << "--------- search John -----------\n";
    Record record = file.search("Julia");
    record.showData();
}

void test(string filename)
{
    StaticHashFile file(filename, N_BUCKETS);
    Record record{};
    ifstream fileIn("data.txt");
    while (true)
    {
        record.setData(fileIn);
        if (fileIn.eof())
            break;
        assert(record == file.search(record.name));
    }
    fileIn.close();
}

int main()
{
    string filename = "data.dat";
    writeFile(filename);
    readFile(filename);
    test(filename);
    return 0;
}
