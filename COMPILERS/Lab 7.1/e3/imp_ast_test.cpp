#include <sstream>
#include <iostream>
#include <iostream>

#include "imp.hh"
#include "imp_parser.hh"
#include "imp_printer.hh"
//#include "imp_interpreter.hh"

int main(int argc, const char* argv[]) {

  bool useparser = true;
  Program *program; 

  if (useparser) {
  
    if (argc != 2) {
      cout << "Incorrect number of arguments" << endl;
      exit(1);
    }
    
    Scanner scanner(argv[1]);
    Parser parser(&scanner);
    program = parser.parse();  // el parser construye la aexp
    
  } else {
    program = new Program();
    Exp* e = new BinaryExp( new NumberExp(2), new NumberExp(3), EXP);
    Stm* s1 = new AssignStatement("x",e);
    Stm* s2 = new PrintStatement(new IdExp("x") );
    program->add(s1);
    program->add(s2);

  }

  ImpPrinter printer;
  //ImpInterpreter interpreter;
  
  printer.print(program);
  //cout << "Run program:" << endl;
  //interpreter.interpret(program);

  delete program;


}
