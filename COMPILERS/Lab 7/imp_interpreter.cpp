#include "imp_interpreter.hh"

int ImpInterpreter::interpret(Program* p) {
    p->accept(this);
    return 0;
}

int ImpInterpreter::visit(Program* p) {
  list<Stm*>::iterator it;
  for (it = p->slist.begin(); it != p->slist.end(); ++it) {
      (*it)->accept(this);
  }
  return 0;
}

int ImpInterpreter::visit(AssignStatement* s) {
  int v = s->rhs->accept(this);
  env.update(s->id, v);
  return 0;
}

int ImpInterpreter::visit(PrintStatement* s) {
  int v = s->e->accept(this);
  cout << v << endl;
  return 0;
}

int ImpInterpreter::visit(BinaryExp* e) {
  int v1 = e->left->accept(this);
  int v2 = e->right->accept(this);
  int result = 0;
  switch(e->op) {
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

int ImpInterpreter::visit(NumberExp* e) {
  return e->value;
}

int ImpInterpreter::visit(Cadena* e) {
    return 1;
}


int ImpInterpreter::visit(IdExp* e) {
  if (env.check(e->id))
    return env.lookup(e->id);
  else {
    cout << "Variable indefinida: " << e->id << endl;
    exit(0);
  }
}

int ImpInterpreter::visit(ParenthExp* ep) {
  return ep->e->accept(this);
}


