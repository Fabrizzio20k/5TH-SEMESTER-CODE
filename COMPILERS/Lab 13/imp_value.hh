#ifndef IMP_VALUE
#define IMP_VALUE

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>

using namespace std;

// esto luego tendra su propia clase
enum ImpVType { NOTYPE=0, TINT, TBOOL }; // value type

class ImpValue {
public:
  ImpValue();
  ImpVType type;
  // podemos ser mas eficientes con la memoria?
  int int_value;
  bool bool_value;
  void set_default_value(ImpVType tt);
  static ImpVType get_basic_type(string s);
};

std::ostream& operator << ( std::ostream& outs, const ImpValue & v );


#endif
