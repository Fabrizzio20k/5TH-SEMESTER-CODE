#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <map>

using namespace std;

int main()
{
    map<string, long> index;
    // add some data
    index["Pedro"] = 1;
    index["Juan"] = 2;
    index["Maria"] = 3;
    index["Jose"] = 4;
    index["Ana"] = 5;

    // print the data
    for (auto it = index.begin(); it != index.end(); it++)
    {
        cout << it->first << " " << it->second << endl;
    }
}