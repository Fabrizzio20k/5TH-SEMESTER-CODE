#ifndef AST_EVALUATOR
#define AST_EVALUATOR

#include "exp.hh"
#include "ast_visitor.hh"

class ASTEvaluator : public ASTVisitor {
public:
  int eval(Exp*);
  // metodos visit

  int visit(BinaryExp *e);
    int visit(NumberExp *e);
    int visit(ParenthExp *e);
};


#endif
