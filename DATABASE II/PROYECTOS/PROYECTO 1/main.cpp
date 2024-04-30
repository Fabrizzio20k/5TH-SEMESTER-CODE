#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

struct BucketRecord
{
    int BucketSize;
    int GlobalDepth;

    string idx;
    vector<long> positions;
    long next;

    friend ostream &operator<<(ostream &os, const BucketRecord &b)
    {
        os.write((char *)&b.idx, b.idx.size());
        for (size_t i = 0; i < b.positions.size(); i++)
        {
            os.write((char *)&b.positions[i], sizeof(long));
        }
        os.write((char *)&b.next, sizeof(long));
        return os;
    }
    friend istream &operator>>(istream &is, BucketRecord &b)
    {
        is.read((char *)&b.idx, b.GlobalDepth);
        for (size_t i = 0; i < b.BucketSize; i++)
        {
            is.read((char *)&b.positions[i], sizeof(long));
        }
        is.read((char *)&b.next, sizeof(long));
        return is;
    }
};

struct Person
{
    string name;
    int age;
    double weight;
};

class ExtendibleHashing
{
public:
    static void clearFile(string file)
    {
        ofstream ofs;
        ofs.open(file, ofstream::out | ofstream::trunc);
        ofs.close();
    }
    ExtendibleHashing(string bucketFile, string directoryFile, int bucketSize, int globalDepth = 3)
    {
        this->bucketFile = bucketFile;
        this->directoryFile = directoryFile;
        this->bucketSize = bucketSize;
        this->globalDepth = globalDepth;

        ExtendibleHashing::clearFile(bucketFile);

        ofstream file(bucketFile);
        // Write the initial buckets
        for (int i = 0; i < 1 << globalDepth; i++)
        {
            BucketRecord bucket;
            bucket.idx = to_string(i);
            bucket.next = -1;
            for (int j = 0; j < bucketSize; j++)
            {
                bucket.positions.push_back(-1);
            }
            file << bucket;
        }
    }
    void insert(Person p);
    void remove(string name);
    void search(string name);
    void display();
    void displayBuckets()
    {
        ifstream file(bucketFile);
        BucketRecord bucket;
        bucket.BucketSize = bucketSize;
        bucket.GlobalDepth = globalDepth;
        while (file >> bucket)
        {
            cout << bucket.idx << " ";
            for (size_t i = 0; i < bucket.positions.size(); i++)
            {
                cout << bucket.positions[i] << " ";
            }
            cout << bucket.next << endl;
        }
    }

private:
    int globalDepth;
    int bucketSize;
    string bucketFile;
    string directoryFile;
};

int main()
{
    ExtendibleHashing eh("index.dat", "data.dat", 2, 5);
    eh.displayBuckets();
}