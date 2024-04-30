

#include "exp.hh"


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


// print
void BinaryExp::print() {
  left->print();
  cout << ' ' << Exp::binopToString(this->op) << ' ';
  right->print();
}

void NumberExp::print() {
  cout << value;
}

void ParenthExp::print() {
  cout << '(';
  e->print();
  cout << ')';
}

// eval
int BinaryExp::eval() {
  int v1 = left->eval();
  int v2 = right->eval();
  int result = 0;
  switch(this->op) {
  case PLUS: result = v1+v2; break;
  case MINUS: result = v1-v2; break;
  case MULT: result = v1 * v2; break;
  case DIV: result = v1 / v2; break;
  case EXP:
    result = 1;
    while (v2 > 0) { result *= v1; v2--; }
    break;
  }
  return result;
}

int NumberExp::eval() {
  return value;
}

int ParenthExp::eval() {
  return e->eval();
}





