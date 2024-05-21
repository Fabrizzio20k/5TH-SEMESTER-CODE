#ifndef IMP_PRINTER
#define IMP_PRINTER

#include "imp.hh"
#include "imp_visitor.hh"

class ImpPrinter : public ImpVisitor {
public:
  void print(Program*);
  void visit(Program*);
  void visit(StatementList*);
  void visit(AssignStatement*);
  void visit(PrintStatement*);
  
  int visit(BinaryExp* e);
  int visit(NumberExp* e);
  int visit(IdExp* e);
  int visit(ParenthExp* e);
};


#endif

