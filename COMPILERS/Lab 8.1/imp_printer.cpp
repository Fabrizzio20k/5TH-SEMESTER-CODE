#include "imp_printer.hh"

void ImpPrinter::print(Program* p) {
  cout << "Program : ";
  p->accept(this);
  return;
}

void ImpPrinter::visit(Program* p) {
  p->slist->accept(this);
  return;
}

void ImpPrinter::visit(StatementList* s) {
  cout << "{" << endl;
  list<Stm*>::iterator it;
  for (it = s->slist.begin(); it != s->slist.end(); ++it) {
    (*it)->accept(this);
    cout << ";" << endl;
  }
  cout << "}" << endl;
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
  cout << "if (";
  s->cond->accept(this);
  cout << ") then" << endl;;
  s->tsl->accept(this);
  if (s->fsl!=NULL) {
    cout << "else" << endl;
    s->fsl->accept(this);
  }
  cout << "endif";
  return;
}

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
