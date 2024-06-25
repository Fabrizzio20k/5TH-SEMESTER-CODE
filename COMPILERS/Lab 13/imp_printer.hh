#ifndef IMP_PRINTER
#define IMP_PRINTER

#include "imp.hh"
#include "imp_visitor.hh"

class ImpPrinter : public ImpVisitor {
public:
  void print(Program*);
  void visit(Program*);
  void visit(Body*);
  void visit(VarDecList*);
  void visit(VarDec*);
  void visit(FunDecList*);
  void visit(FunDec*);  
  void visit(StatementList*);
  void visit(AssignStatement*);
  void visit(PrintStatement*);
  void visit(IfStatement*);
  void visit(WhileStatement*);
  void visit(ReturnStatement*);

  int visit(BinaryExp* e);
  int visit(NumberExp* e);
  int visit(TrueFalseExp* e);
  int visit(IdExp* e);
  int visit(ParenthExp* e);
  int visit(CondExp* e);
  int visit(FCallExp* e);
};


#endif

