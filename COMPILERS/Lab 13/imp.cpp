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
TrueFalseExp::TrueFalseExp(bool v):value(v) {}
IdExp::IdExp(string id):id(id) {}
ParenthExp::ParenthExp(Exp *e):e(e){}
CondExp::CondExp(Exp *c, Exp* et, Exp* ef):cond(c), etrue(et), efalse(ef){}
FCallExp::FCallExp(string fname, list<Exp*> args):fname(fname), args(args) {}

Exp::~Exp() {}
BinaryExp::~BinaryExp() { delete left; delete right; }
NumberExp::~NumberExp() { }
TrueFalseExp::~TrueFalseExp() { }
IdExp::~IdExp() { }
ParenthExp::~ParenthExp(){ delete e; }
CondExp::~CondExp(){ delete cond; delete etrue; delete efalse; }
FCallExp::~FCallExp(){
  while (!args.empty()) { delete args.front();  ; args.pop_front();  }
}

int BinaryExp::accept(ImpVisitor* v) {
  return v->visit(this);
}

int NumberExp::accept(ImpVisitor* v) {
  return v->visit(this);
}

int TrueFalseExp::accept(ImpVisitor* v) {
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

int FCallExp::accept(ImpVisitor* v) {
  return v->visit(this);
}



// value visitor

ImpValue BinaryExp::accept(ImpValueVisitor* v) {
  return v->visit(this);
}

ImpValue TrueFalseExp::accept(ImpValueVisitor* v) {
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

ImpValue FCallExp::accept(ImpValueVisitor* v) {
  return v->visit(this);
}

// type visitor

ImpType BinaryExp::accept(TypeVisitor* v) {
  return v->visit(this);
}

ImpType TrueFalseExp::accept(TypeVisitor* v) {
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

ImpType FCallExp::accept(TypeVisitor* v) {
  return v->visit(this);
}

AssignStatement::AssignStatement(string id, Exp* e):id(id), rhs(e) { }
PrintStatement::PrintStatement(Exp* e):e(e) { }
IfStatement::IfStatement(Exp* c,Body *tb, Body* fb):cond(c),tbody(tb), fbody(fb) { }
WhileStatement::WhileStatement(Exp* c,Body *b):cond(c),body(b) { }
ReturnStatement::ReturnStatement(Exp* e):e(e) { }

StatementList::StatementList():slist() {}
VarDec::VarDec(string type, list<string> vars):type(type), vars(vars) {}
FunDec::FunDec(string fname, list<string> types, list<string> vars, string rtype, Body* body): fname(fname), vars(vars), types(types), rtype(rtype), body(body) { is_main = false; }
VarDecList::VarDecList():vdlist() {}
FunDecList::FunDecList():fdlist() {}
Body::Body(VarDecList* vdl, StatementList* sl):var_decs(vdl), slist(sl) {}
Program::Program(VarDecList* vdl, FunDecList* fdl):var_decs(vdl), fun_decs(fdl) {}

Stm::~Stm() {}
AssignStatement::~AssignStatement() { delete rhs; }
PrintStatement::~PrintStatement() { delete e; }
IfStatement::~IfStatement() { delete fbody; delete tbody; delete cond; }
WhileStatement::~WhileStatement() { delete body; delete cond; }
ReturnStatement::~ReturnStatement() { delete e; }

StatementList::~StatementList() { }
VarDec::~VarDec() { }
FunDec::~FunDec() { }
VarDecList::~VarDecList() { }
FunDecList::~FunDecList() { }
Body::~Body() { delete slist; delete var_decs; }
Program::~Program() { delete fun_decs; delete var_decs; }

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

void ReturnStatement::accept(ImpVisitor* v) {
  return v->visit(this);
}

void StatementList::add(Stm* s) { slist.push_back(s);  }

void StatementList::accept(ImpVisitor* v) {
  return v->visit(this);
}

void VarDec::accept(ImpVisitor* v) {
  return v->visit(this);
}

void FunDec::accept(ImpVisitor* v) {
  return v->visit(this);
}

void VarDecList::add(VarDec* vd) { vdlist.push_back(vd);  }

void VarDecList::accept(ImpVisitor* v) {
  return v->visit(this);
}

void FunDecList::add(FunDec* vd) { fdlist.push_back(vd);  }

void FunDecList::accept(ImpVisitor* v) {
  return v->visit(this);
}

void Body::accept(ImpVisitor* v) {
  return v->visit(this);
}

void Program::accept(ImpVisitor* v) {
  return v->visit(this);
}

// Value visitor

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

void ReturnStatement::accept(ImpValueVisitor* v) {
  return v->visit(this);
}


void StatementList::accept(ImpValueVisitor* v) {
  return v->visit(this);
}

void VarDec::accept(ImpValueVisitor* v) {
  return v->visit(this);
}

void FunDec::accept(ImpValueVisitor* v) {
  return v->visit(this);
}

void VarDecList::accept(ImpValueVisitor* v) {
  return v->visit(this);
}

void FunDecList::accept(ImpValueVisitor* v) {
  return v->visit(this);
}

void Body::accept(ImpValueVisitor* v) {
  return v->visit(this);
}

void Program::accept(ImpValueVisitor* v) {
  return v->visit(this);
}

// Type visitor

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

void ReturnStatement::accept(TypeVisitor* v) {
  return v->visit(this);
}


void StatementList::accept(TypeVisitor* v) {
  return v->visit(this);
}

void VarDec::accept(TypeVisitor* v) {
  return v->visit(this);
}

void FunDec::accept(TypeVisitor* v) {
  return v->visit(this);
}

void VarDecList::accept(TypeVisitor* v) {
  return v->visit(this);
}

void FunDecList::accept(TypeVisitor* v) {
  return v->visit(this);
}

void Body::accept(TypeVisitor* v) {
  return v->visit(this);
}

void Program::accept(TypeVisitor* v) {
  return v->visit(this);
}









