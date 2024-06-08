#ifndef IMP_INTERPRETER
#define IMP_INTERPRETER

#include <unordered_map>

#include "imp.hh"
#include "imp_value_visitor.hh"
#include "environment.hh"

using namespace std;

class ImpInterpreter : public ImpValueVisitor {
private:
  Environment<ImpValue> env;
public:
  void interpret(Program*);
  void visit(Program*);
  void visit(Body*);
  void visit(VarDecList*);
  void visit(VarDec*);  
  void visit(StatementList*);
  void visit(AssignStatement*);
  void visit(PrintStatement*);
  void visit(IfStatement*);
  void visit(WhileStatement*);
  
  ImpValue visit(BinaryExp* e);
  ImpValue visit(NumberExp* e);
  ImpValue visit(IdExp* e);
  ImpValue visit(ParenthExp* e);
  ImpValue visit(CondExp* e);
};


#endif

