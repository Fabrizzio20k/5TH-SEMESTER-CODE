#include <sstream>
#include <iostream>
#include <iostream>

#include "imp.hh"
#include "imp_parser.hh"
#include "imp_printer.hh"
#include "imp_interpreter.hh"

int main(int argc, const char *argv[])
{

  bool useparser = true;
  Program *program;

  if (useparser)
  {

    if (argc != 2)
    {
      cout << "Incorrect number of arguments" << endl;
      exit(1);
    }

    std::ifstream t(argv[1]);
    std::stringstream buffer;
    buffer << t.rdbuf();
    Scanner scanner(buffer.str());

    // Scanner scanner(argv[1]);
    Parser parser(&scanner);
    program = parser.parse(); // el parser construye la aexp
  }
  else
  {
    // Exp* e = new BinaryExp( new NumberExp(2), new NumberExp(3), EXP);
    // Stm* s1 = new AssignStatement("x",e);
    // Stm* s2 = new PrintStatement(new IdExp("x") );
    // StatementList* sl = new StatementList();
    // sl->add(s1);
    // sl->add(s2);
    // program = new Program(sl);
  }

  ImpPrinter printer;
  ImpInterpreter interpreter;

  printer.print(program);
  cout << "Run program:" << endl;
  interpreter.interpret(program);

  delete program;
}
