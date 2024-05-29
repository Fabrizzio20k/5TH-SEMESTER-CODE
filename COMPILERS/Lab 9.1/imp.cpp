#include "imp.hh"
#include "imp_visitor.hh"

string Exp::binopToString(BinaryOp op)
{
  switch (op)
  {
  case PLUS:
    return "+";
  case MINUS:
    return "-";
  case MULT:
    return "*";
  case DIV:
    return "/";
  case EXP:
    return "**";
  case LT:
    return "<";
  case LTEQ:
    return "<=";
  case EQ:
    return "==";
  }
  return "";
}

// Constructors
BinaryExp::BinaryExp(Exp *l, Exp *r, BinaryOp op) : left(l), right(r), op(op) {}
NumberExp::NumberExp(int v) : value(v) {}
IdExp::IdExp(string id) : id(id) {}
ParenthExp::ParenthExp(Exp *e) : e(e) {}
CondExp::CondExp(Exp *c, Exp *et, Exp *ef) : cond(c), etrue(et), efalse(ef) {}

Exp::~Exp() {}
BinaryExp::~BinaryExp()
{
  delete left;
  delete right;
}
NumberExp::~NumberExp() {}
IdExp::~IdExp() {}
ParenthExp::~ParenthExp() { delete e; }
CondExp::~CondExp()
{
  delete cond;
  delete etrue;
  delete efalse;
}

int BinaryExp::accept(ImpVisitor *v)
{
  return v->visit(this);
}

int NumberExp::accept(ImpVisitor *v)
{
  return v->visit(this);
}

int IdExp::accept(ImpVisitor *v)
{
  return v->visit(this);
}

int ParenthExp::accept(ImpVisitor *v)
{
  return v->visit(this);
}

int CondExp::accept(ImpVisitor *v)
{
  return v->visit(this);
}

AssignStatement::AssignStatement(string id, Exp *e) : id(id), rhs(e) {}
PrintStatement::PrintStatement(Exp *e) : e(e) {}
IfStatement::IfStatement(Exp *c, StatementList *tsl, StatementList *fsl) : cond(c), tsl(tsl), fsl(fsl) {}
WhileStatement::WhileStatement(Exp *c, StatementList *sl) : cond(c), sl(sl) {}

StatementList::StatementList() : slist() {}
Program::Program(Body *b) : body(b) {}
Body::Body(VarDecList *var_decs, StatementList *sl) : var_decs(var_decs), slist(slist) {}
VarDecList::VarDecList() : vdlist() {}
VarDec::VarDec(string type, list<string> vars) : type(type), vars(vars) {}

Stm::~Stm() {}
AssignStatement::~AssignStatement() { delete rhs; }
PrintStatement::~PrintStatement() { delete e; }
IfStatement::~IfStatement()
{
  delete fsl;
  delete tsl;
  delete cond;
}
WhileStatement::~WhileStatement()
{
  delete sl;
  delete cond;
}

StatementList::~StatementList() {}
VarDecList::~VarDecList()
{
  for (auto vd : vdlist)
  {
    delete vd;
  }
}
VarDec::~VarDec() {}
Body::~Body()
{
  delete var_decs;
  delete slist;
}
Program::~Program()
{
  // loop
}

void AssignStatement::accept(ImpVisitor *v)
{
  return v->visit(this);
}

void PrintStatement::accept(ImpVisitor *v)
{
  return v->visit(this);
}

void IfStatement::accept(ImpVisitor *v)
{
  return v->visit(this);
}

void WhileStatement::accept(ImpVisitor *v)
{
  return v->visit(this);
}

void StatementList::add(Stm *s) { slist.push_back(s); }

void StatementList::accept(ImpVisitor *v)
{
  return v->visit(this);
}

void VarDecList::add(VarDec *s) { vdlist.push_back(s); }

int VarDecList::accept(ImpVisitor *v)
{
  return v->visit(this);
}

int VarDec::accept(ImpVisitor *v)
{
  return v->visit(this);
}

int Body::accept(ImpVisitor *v)
{
  return v->visit(this);
}

void Program::accept(ImpVisitor *v)
{
  return v->visit(this);
}
