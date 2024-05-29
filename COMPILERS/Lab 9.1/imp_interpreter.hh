#ifndef IMP_INTERPRETER
#define IMP_INTERPRETER

#include <unordered_map>

#include "imp.hh"
#include "imp_visitor.hh"
#include "environment.hh"

using namespace std;

class ImpInterpreter : public ImpVisitor
{
private:
  Environment<int> env;

public:
  void interpret(Program *);
  int visit(Body *);
  int visit(VarDecList *);
  int visit(VarDec *);
  void visit(Program *);
  void visit(StatementList *);
  void visit(AssignStatement *);
  void visit(PrintStatement *);
  void visit(IfStatement *);
  void visit(WhileStatement *);

  int visit(BinaryExp *e);
  int visit(NumberExp *e);
  int visit(IdExp *e);
  int visit(ParenthExp *e);
  int visit(CondExp *e);
};

#endif
