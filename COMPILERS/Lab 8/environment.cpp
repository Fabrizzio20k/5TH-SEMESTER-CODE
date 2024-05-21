#include "environment.hh"


Environment::Environment() {

}


void Environment::update(string x, int v) {
  level[x] = v;
}

bool Environment::check(string x) {
  unordered_map<std::string,int>::const_iterator it = level.find(x);
  if (it == level.end())
    return false;
 else
   return true;
}

int Environment::lookup(string x) {
  unordered_map<std::string,int>::const_iterator it = level.find(x);
  if (it == level.end())
    return 0;
 else
   return it->second;
}
