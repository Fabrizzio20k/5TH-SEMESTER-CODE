#include "imp_interpreter.hh"

/* *************** Manejo del estado del programa ****** */

void ImpInterpreter::memoria_update(string x, int v) {
  memoria[x] = v;
}
bool ImpInterpreter::memoria_check(string x) {
  std::unordered_map<std::string,int>::const_iterator it = memoria.find(x);
  if (it == memoria.end())
    return false;
 else
   return true;
}
int ImpInterpreter::memoria_lookup(string x) {
  std::unordered_map<std::string,int>::const_iterator it = memoria.find(x);
  if (it == memoria.end())
    return 0;
 else
   return it->second;
}

/* *************** Fin manejo del estado del programa ****** */

int ImpInterpreter::interpret(Program* p) {
    return p->accept(this);
}

int ImpInterpreter::visit(Program* p) {
    for (auto & it : p->slist) {
        it->accept(this);
    }
    return 0;
}

int ImpInterpreter::visit(AssignStatement* s) {
    int v = s->rhs->accept(this);
    memoria_update(s->id, v);
    return 0;
}

int ImpInterpreter::visit(PrintStatement* s) {
    cout << s->e->accept(this) << endl;
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

int ImpInterpreter::visit(IdExp* e) {
  if (memoria_check(e->id))
    return memoria_lookup(e->id);
  else {
    cout << "Variable indefinida: " << e->id << endl;
    exit(0);
  }
}

int ImpInterpreter::visit(ParenthExp* ep) {
  return ep->e->accept(this);
}
