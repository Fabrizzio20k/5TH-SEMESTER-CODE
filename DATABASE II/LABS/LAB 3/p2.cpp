#include <functional>
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

template <typename T>
size_t hashRecord(T t)
{
    return hash<T>()(t);
}

struct Record
{
    int key;
    string value;
};

struct IndexRecord
{
    int index;
    vector<int> keys;
    int next;
};

class StaticHash
{
private:
    long buckets;
    string indexFileName;
    IndexRecord getDataFromBucket(int bucket)
    {
        fstream indexFile(indexFileName, ios::in | ios::out | ios::binary);
        IndexRecord indexRecord;
        indexFile.seekg(bucket * sizeof(IndexRecord));
        for (int i = 0; i < 5; i++)
        {
            int key;
            indexFile.read((char *)&key, sizeof(int));
            if (key != -1)
            {
                indexRecord.keys.push_back(key);
            }
        }
        indexFile.read((char *)&indexRecord.next, sizeof(int));
        indexFile.close();
        return indexRecord;
    }

public:
    static void clearIndexFile(string indexFileName)
    {
        fstream indexFile(indexFileName, ios::in | ios::out | ios::binary);
        if (!indexFile)
        {
            indexFile.open(indexFileName, ios::out | ios::binary);
            indexFile.close();
        }
        else
        {
            indexFile.close();
            remove(indexFileName.c_str());
        }
    }
    StaticHash(long buckets) : buckets(buckets)
    {
        fstream indexFile(indexFileName, ios::in | ios::out | ios::binary);
        if (!indexFile)
        {
            indexFile.open(indexFileName, ios::out | ios::binary);
            for (int i = 0; i < buckets; i++)
            {
                IndexRecord indexRecord;
                indexRecord.index = i;
                for (int j = 0; j < 5; j++)
                {
                    indexRecord.keys.push_back(-1);
                }
                indexRecord.next = -1;

                // Write index, next and keys to file
                indexFile.write((char *)&indexRecord.index, sizeof(int));
                for (int key : indexRecord.keys)
                {
                    indexFile.write((char *)&key, sizeof(int));
                }
                indexFile.write((char *)&indexRecord.next, sizeof(int));
            }
        }
        indexFile.close();
    }
    void insert(Record record)
    {
        size_t hash = hashRecord(record.key) % buckets;
    }
    void printDataFromBuckets()
    {
        for (int i = 0; i < buckets; i++)
        {
            IndexRecord indexRecord = getDataFromBucket(i);
            cout << "Bucket: " << indexRecord.index << endl;
            cout << "Keys: ";
            for (int key : indexRecord.keys)
            {
                cout << key << " ";
            }
            cout << endl;
            cout << "Next: " << indexRecord.next << endl;
        }
    }
};

int main()
{
    StaticHash::clearIndexFile("index.dat");
    StaticHash staticHash(10);
    staticHash.printDataFromBuckets();
}