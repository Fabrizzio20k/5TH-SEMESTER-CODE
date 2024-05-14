#include "imp.hh"
#include "imp_ast_visitor.hh"

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
IdExp::IdExp(string id):id(id) {}
ParenthExp::ParenthExp(Exp *e):e(e){}

Exp::~Exp() {}
BinaryExp::~BinaryExp() { delete left; delete right; }
NumberExp::~NumberExp() { }
IdExp::~IdExp() { }
ParenthExp::~ParenthExp(){ delete e; }

int BinaryExp::accept(ImpASTVisitor* v) {
  return v->visit(this);
}

int NumberExp::accept(ImpASTVisitor* v) {
  return v->visit(this);
}

int IdExp::accept(ImpASTVisitor* v) {
  return v->visit(this);
}

int ParenthExp::accept(ImpASTVisitor* v) {
  return v->visit(this);
}

AssignStatement::AssignStatement(string id, Exp* e):id(id), rhs(e) { }
PrintStatement::PrintStatement(Exp* e):e(e) { }
Program::Program():slist() {}

Stm::~Stm() {}
AssignStatement::~AssignStatement() { delete rhs; }
PrintStatement::~PrintStatement() { delete e; }
Program::~Program() {
  // loop 
}

int AssignStatement::accept(ImpASTVisitor* v) {
  return v->visit(this);
}

int PrintStatement::accept(ImpASTVisitor* v) {
  return v->visit(this);
}

void Program::add(Stm* s) { slist.push_back(s);  }

int Program::accept(ImpASTVisitor* v) {
  return v->visit(this);
}








