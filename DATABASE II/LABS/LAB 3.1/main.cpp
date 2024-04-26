#include "HashFile.h"

using namespace std;

const int N_BUCKETS = 4;

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
    cout << "--------- search Pancho -----------\n";
    Record record = file.search("Dina");
    record.showData();
}

int main()
{
    StaticHashFile::clearFile("data.dat");
    writeFile("data.dat");
    readFile("data.dat");
    return 0;
}