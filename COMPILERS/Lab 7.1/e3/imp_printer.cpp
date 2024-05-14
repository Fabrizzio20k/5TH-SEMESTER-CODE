#include "imp_printer.hh"

void ImpPrinter::print(Program* p) {
  cout << "Program : ";
  p->accept(this);
  return;
}

int ImpPrinter::visit(Program* p) {
  cout << "{" << endl;
  list<Stm*>::iterator it;
  for (it = p->slist.begin(); it != p->slist.end(); ++it) {
    (*it)->accept(this);
    cout << ";" << endl;
  }
  cout << "}" << endl;
  return 0;
}

int ImpPrinter::visit(AssignStatement* s) {
  cout << s->id << " = ";
  s->rhs->accept(this);
  return 0;
}

int ImpPrinter::visit(PrintStatement* s) {
  cout << "print(";
  s->e->accept(this);
  cout << ")";
  return 0;
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
