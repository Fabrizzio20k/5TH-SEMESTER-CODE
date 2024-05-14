#ifndef AST_PRINTER
#define AST_PRINTER

#include "exp.hh"
#include "ast_visitor.hh"

class ASTPrinter : public ASTVisitor {
public:
  void print(Exp*);
  int visit(BinaryExp* e);
  int visit(NumberExp* e);
  int visit(ParenthExp* e);
};


#endif

