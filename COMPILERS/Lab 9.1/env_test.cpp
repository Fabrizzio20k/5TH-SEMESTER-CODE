#include "environment.hh"

#include <iostream>

int main(int argc, const char *argv[])
{

  Environment<int> env;

  env.add_level();
  env.add_var("x", 10);
  env.add_var("y", 20);

  env.add_level();
  env.add_var("x", 0);
  env.update("x", 100);

  cout << "x = " << env.lookup("x") << endl;
  cout << "y = " << env.lookup("y") << endl;

  env.update("y", 200);
  env.remove_level();
  cout << "x = " << env.lookup("x") << endl;
  cout << "y = " << env.lookup("y") << endl;
}
