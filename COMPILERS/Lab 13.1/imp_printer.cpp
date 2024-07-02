#include "imp_printer.hh"

void ImpPrinter::print(Program* p) {
  p->accept(this);
  return;
}

void ImpPrinter::visit(Program* p) {
  p->var_decs->accept(this);
  p->fun_decs->accept(this);
  return;
}

void ImpPrinter::visit(Body * b) {
  b->var_decs->accept(this);
  b->slist->accept(this);
  return;
}

void ImpPrinter::visit(VarDecList* s) {
  list<VarDec*>::iterator it;
  for (it = s->vdlist.begin(); it != s->vdlist.end(); ++it) {
    (*it)->accept(this);
    cout << ";" << endl;
  }  
  return;
}

void ImpPrinter::visit(FunDecList* s) {
  list<FunDec*>::iterator it;
  for (it = s->fdlist.begin(); it != s->fdlist.end(); ++it) {
    (*it)->accept(this);
    cout << endl;
  }
  return;
}

void ImpPrinter::visit(FunDec* fd) {
  cout << "fun " << fd->rtype << " " << fd->fname << "(";
  bool first = true;
  list<string>::iterator it, vit;
  for (it = fd->types.begin(), vit = fd->vars.begin();
       it != fd->types.end(); ++it, ++vit){
    if (!first) cout << ", ";
    first = false;
    cout << *it << " " << *vit;
  }
  cout << ")" << endl;
  fd->body->accept(this);
  cout << "endfun";
  return;
}
			  
void ImpPrinter::visit(VarDec* vd) {
  bool first = true;
  cout << "var " << vd->type << " ";
  list<string>::iterator it;
  for (it = vd->vars.begin(); it != vd->vars.end(); ++it){
    if (!first) cout << ", ";
    first = false;
    cout << *it;
  }
  return;
}

void ImpPrinter::visit(StatementList* s) {
  list<Stm*>::iterator it;
  for (it = s->slist.begin(); it != s->slist.end(); ++it) {
    (*it)->accept(this);
    cout << ";" << endl;
  }
  return;
}

void ImpPrinter::visit(AssignStatement* s) {
  cout << s->id << " = ";
  s->rhs->accept(this);
  return;
}

void ImpPrinter::visit(PrintStatement* s) {
  cout << "print(";
  s->e->accept(this);
  cout << ")";
  return;
}

void ImpPrinter::visit(IfStatement* s) {
  cout << "if";
  s->cond->accept(this);
  cout << " then" << endl;;
  s->tbody->accept(this);
  if (s->fbody!=NULL) {
    cout << "else" << endl;
    s->fbody->accept(this);
  }
  cout << "endif";
  return;
}

void ImpPrinter::visit(WhileStatement* s) {
  cout << "while ";
  s->cond->accept(this);
  cout << "do" << endl;;
  s->body->accept(this);
  cout << "endwhile";
  return;
}

void ImpPrinter::visit(ReturnStatement* s) {
  cout << "return (";
  if (s->e != NULL) s->e->accept(this);
  cout << ")";
  return;
}

// Expresiones

int ImpPrinter::visit(BinaryExp* e) {
  e->left->accept(this);
  cout << ' ' << Exp::binopToString(e->op) << ' ';
  e->right->accept(this);
  return 0;
}

int ImpPrinter::visit(NumberExp* e) {
  cout << e->value;
  return 0;
}

int ImpPrinter::visit(TrueFalseExp* e) {
  if (e->value) cout << "true";
  else cout << "false";
  return 0;
}

int ImpPrinter::visit(IdExp* e) {
  cout << e->id;
  return 0;
}

int ImpPrinter::visit(ParenthExp* ep) {
  cout << '(';
  ep->e->accept(this);
  cout << ')';
  return 0;
}

int ImpPrinter::visit(CondExp* e) {
  cout << "ifexp(";
  e->cond->accept(this);
  cout << ",";
  e->etrue->accept(this);
  cout << ",";
  e->efalse->accept(this);
  cout << ')';
  return 0;
}

int ImpPrinter::visit(FCallExp* e) {
  cout << e->fname << "(";
  list<Exp*>::iterator it;
  bool first = true;
  for (it = e->args.begin(); it != e->args.end(); ++it) {
    if (!first) cout << ",";
    first = false;
    (*it)->accept(this);
    
  }
  cout << ')';
  return 0;
}
