#include "environment.hh"
#include "imp_value.hh"

#include <iostream>

int main(int argc, const char* argv[]) {

  Environment<int> env;
  
  env.add_level();
  env.add_var("x",10);
  env.add_var("y",20);
  env.add_level();
  env.add_var("x",0);
  env.update("x",100);
  //env.update("z",10000);
  cout << "x = " << env.lookup("x") << endl;
  cout << "y = " << env.lookup("y") << endl;
  //cout << "z = " << env.lookup("z") << endl;
  env.update("y", 200);
  env.remove_level();
  cout << "x = " << env.lookup("x") << endl;
  cout << "y = " << env.lookup("y") << endl;

  ImpValue v;
  cout << v << endl;
  ImpType tt = ImpValue::get_basic_type("int");
  if (tt == TINT) cout << "entero" << endl;

  v.set_default_value(ImpValue::get_basic_type("int"));
  cout << v << endl;
  v.set_default_value(ImpValue::get_basic_type("bool"));
  cout << v << endl;
  v.set_default_value(ImpValue::get_basic_type("xxx"));
  cout << v << endl;

  ImpValue x;
  x.set_default_value(TINT);
  x.int_value = 20;
  cout << x << endl;
  
}
