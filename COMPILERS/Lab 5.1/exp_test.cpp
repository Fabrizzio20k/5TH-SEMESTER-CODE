#include <sstream>
#include <iostream>
#include <iostream>

#include "exp.hh"
#include "exp_parser.hh"

int main(int argc, const char *argv[])
{

  bool useparser = true;
  Exp *exp;

  if (useparser)
  {

    if (argc != 2)
    {
      cout << "Incorrect number of arguments" << endl;
      exit(1);
    }
    Scanner scanner(argv[1]);
    Parser parser(&scanner);
    exp = parser.parse(); // el parser construye la aexp
  }
  else
  { // contruyendo ASTs a mano

    exp = new BinaryExp(new NumberExp(2), new NumberExp(3), EXP);
  }

  if (exp != NULL)
  {
    cout << "expr: ";
    exp->print();
    cout << endl;

    cout << "eval: ";
    cout << exp->eval() << endl;

    delete exp;
  }
}
