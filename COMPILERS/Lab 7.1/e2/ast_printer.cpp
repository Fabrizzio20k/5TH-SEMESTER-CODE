#include "ast_printer.hh"

void ASTPrinter::print(Exp* e) {
  cout << "expression: ";
  e->accept(this);
  cout << endl;
  return;
}

int ASTPrinter::visit(BinaryExp* e) {
  e->left->accept(this);
  cout << ' ' << Exp::binopToString(e->op) << ' ';
  e->right->accept(this);
  return 0;
}

int ASTPrinter::visit(NumberExp* e) {
  cout << e->value;
  return 0;
}

int ASTPrinter::visit(ParenthExp* ep) {
  cout << '(';
  ep->e->accept(this);
  cout << ')';
  return 0;
}
