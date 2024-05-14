#ifndef IMP_PRINTER
#define IMP_PRINTER

#include "imp.hh"
#include "imp_ast_visitor.hh"

class ImpPrinter : public ImpASTVisitor {
public:
  void print(Program*);
  int visit(Program*);
  int visit(AssignStatement*);
  int visit(PrintStatement*);
  
  int visit(BinaryExp* e);
  int visit(NumberExp* e);
  int visit(IdExp* e);
  int visit(ParenthExp* e);
};


#endif

