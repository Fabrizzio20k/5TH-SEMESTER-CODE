#include "imp.hh"
#include "imp_visitor.hh"
#include "imp_value_visitor.hh"
#include "type_visitor.hh"

string Exp::binopToString(BinaryOp op) {
  switch(op) {
  case PLUS: return "+";
  case MINUS: return "-";
  case MULT: return "*";
  case DIV: return "/";
  case EXP: return "**";
  case LT: return "<";
  case LTEQ: return "<=";
  case EQ: return "==";
  }
  return "";
}


// Constructors
BinaryExp::BinaryExp(Exp* l, Exp* r, BinaryOp op):left(l),right(r),op(op) {}
NumberExp::NumberExp(int v):value(v) {}
IdExp::IdExp(string id):id(id) {}
ParenthExp::ParenthExp(Exp *e):e(e){}
CondExp::CondExp(Exp *c, Exp* et, Exp* ef):cond(c), etrue(et), efalse(ef){}

Exp::~Exp() {}
BinaryExp::~BinaryExp() { delete left; delete right; }
NumberExp::~NumberExp() { }
IdExp::~IdExp() { }
ParenthExp::~ParenthExp(){ delete e; }
CondExp::~CondExp(){ delete cond; delete etrue; delete efalse; }

// ImpVisitor
int BinaryExp::accept(ImpVisitor* v) {
  return v->visit(this);
}
int NumberExp::accept(ImpVisitor* v) {
  return v->visit(this);
}
int IdExp::accept(ImpVisitor* v) {
  return v->visit(this);
}
int ParenthExp::accept(ImpVisitor* v) {
  return v->visit(this);
}
int CondExp::accept(ImpVisitor* v) {
  return v->visit(this);
}

// ImpValueVisitor
ImpValue BinaryExp::accept(ImpValueVisitor* v) {
  return v->visit(this);
}
ImpValue NumberExp::accept(ImpValueVisitor* v) {
  return v->visit(this);
}
ImpValue IdExp::accept(ImpValueVisitor* v) {
  return v->visit(this);
}
ImpValue ParenthExp::accept(ImpValueVisitor* v) {
  return v->visit(this);
}
ImpValue CondExp::accept(ImpValueVisitor* v) {
  return v->visit(this);
}

// TypeVisitor
ImpType BinaryExp::accept(TypeVisitor* v) {
  return v->visit(this);
}
ImpType NumberExp::accept(TypeVisitor* v) {
  return v->visit(this);
}
ImpType IdExp::accept(TypeVisitor* v) {
  return v->visit(this);
}
ImpType ParenthExp::accept(TypeVisitor* v) {
  return v->visit(this);
}
ImpType CondExp::accept(TypeVisitor* v) {
  return v->visit(this);
}

AssignStatement::AssignStatement(string id, Exp* e):id(id), rhs(e) { }
PrintStatement::PrintStatement(Exp* e):e(e) { }
IfStatement::IfStatement(Exp* c,Body *tb, Body* fb):cond(c),tbody(tb), fbody(fb) { }
WhileStatement::WhileStatement(Exp* c,Body *b):cond(c),body(b) { }

StatementList::StatementList():slist() {}
VarDec::VarDec(string type, list<string> vars):type(type), vars(vars) {}
VarDecList::VarDecList():vdlist() {}
Body::Body(VarDecList* vdl, StatementList* sl):var_decs(vdl), slist(sl) {}
Program::Program(Body* b):body(b) {}

Stm::~Stm() {}
AssignStatement::~AssignStatement() { delete rhs; }
PrintStatement::~PrintStatement() { delete e; }
IfStatement::~IfStatement() { delete fbody; delete tbody; delete cond; }
WhileStatement::~WhileStatement() { delete body; delete cond; }

StatementList::~StatementList() { }
VarDec::~VarDec() { }
VarDecList::~VarDecList() { }
Body::~Body() { delete slist; delete var_decs; }
Program::~Program() { delete body; }

// ImpVisitor
void AssignStatement::accept(ImpVisitor* v) {
  return v->visit(this);
}
void PrintStatement::accept(ImpVisitor* v) {
  return v->visit(this);
}
void IfStatement::accept(ImpVisitor* v) {
  return v->visit(this);
}
void WhileStatement::accept(ImpVisitor* v) {
  return v->visit(this);
}
void StatementList::add(Stm* s) { slist.push_back(s);  }
void StatementList::accept(ImpVisitor* v) {
  return v->visit(this);
}
void VarDec::accept(ImpVisitor* v) {
  return v->visit(this);
}
void VarDecList::add(VarDec* vd) { vdlist.push_back(vd);  }
void VarDecList::accept(ImpVisitor* v) {
  return v->visit(this);
}
void Body::accept(ImpVisitor* v) {
  return v->visit(this);
}
void Program::accept(ImpVisitor* v) {
  return v->visit(this);
}


// ImpValueVisitor
void AssignStatement::accept(ImpValueVisitor* v) {
  return v->visit(this);
}
void PrintStatement::accept(ImpValueVisitor* v) {
  return v->visit(this);
}
void IfStatement::accept(ImpValueVisitor* v) {
  return v->visit(this);
}
void WhileStatement::accept(ImpValueVisitor* v) {
  return v->visit(this);
}
void StatementList::accept(ImpValueVisitor* v) {
  return v->visit(this);
}
void VarDec::accept(ImpValueVisitor* v) {
  return v->visit(this);
}
void VarDecList::accept(ImpValueVisitor* v) {
  return v->visit(this);
}
void Body::accept(ImpValueVisitor* v) {
  return v->visit(this);
}
void Program::accept(ImpValueVisitor* v) {
  return v->visit(this);
}

// TypeVisitor
void AssignStatement::accept(TypeVisitor* v) {
  return v->visit(this);
}
void PrintStatement::accept(TypeVisitor* v) {
  return v->visit(this);
}
void IfStatement::accept(TypeVisitor* v) {
  return v->visit(this);
}
void WhileStatement::accept(TypeVisitor* v) {
  return v->visit(this);
}
void StatementList::accept(TypeVisitor* v) {
  return v->visit(this);
}
void VarDec::accept(TypeVisitor* v) {
  return v->visit(this);
}
void VarDecList::accept(TypeVisitor* v) {
  return v->visit(this);
}
void Body::accept(TypeVisitor* v) {
  return v->visit(this);
}
void Program::accept(TypeVisitor* v) {
  return v->visit(this);
}






