
#include "exp.hh"
#include "ast_visitor.hh"


string Exp::binopToString(BinaryOp op) {
  switch(op) {
  case PLUS: return "+";
  case MINUS: return "-";
  case MULT: return "*";
  case DIV: return "/";
  case EXP: return "**";
  }
  return "";
}


// Constructors
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp op):left(l),right(r),op(op) {}
NumberExp::NumberExp(int v):value(v) {}
ParenthExp::ParenthExp(Exp *e):e(e){}

Exp::~Exp() {}
BinaryExp::~BinaryExp() { delete left; delete right; }
NumberExp::~NumberExp() { }
ParenthExp::~ParenthExp(){ delete e; }

int BinaryExp::accept(ASTVisitor* v) {
  return v->visit(this);
}

int NumberExp::accept(ASTVisitor* v) {
  return v->visit(this);
}

int ParenthExp::accept(ASTVisitor* v) {
  return v->visit(this);
}







