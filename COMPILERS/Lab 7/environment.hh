#ifndef ENV
#define ENV

#include <unordered_map>
#include <list>
#include <string>

using namespace std;

class Environment {
private:
    unordered_map<string, int> level;
public:
    Environment();
    void update(string x, int v);
    bool check(string x);
    int  lookup(string x);
};

#endif