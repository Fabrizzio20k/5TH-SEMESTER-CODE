#include <sstream>
#include <iostream>
#include <iostream>

#include "exp_parser.hh"

#include "exp.hh"

using namespace std;

int main(int argc, const char* argv[]) {

  bool useparser = true;
  Exp* exp=NULL;

  if (useparser) {
  
    if (argc != 2) {
      cout << "Incorrect number of arguments" << endl;
      exit(1);
    }
    
    Scanner scanner(argv[1]);
    Parser parser(&scanner);
    exp = parser.parse();  // el parser construye la aexp
    
  } else {
 
    exp = new BinaryExp( new NumberExp(2), new NumberExp(3), EXP);

  }

  cout << "expression: ";
  exp->print();
  cout << endl;
  cout << "eval: " << exp->eval() << endl;
  

  delete exp;
    

}
