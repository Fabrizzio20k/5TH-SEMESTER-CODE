#include "imp_interpreter.hh"


/* *************** Fin manejo del estado del programa ****** */

void ImpInterpreter::interpret(Program* p) {
  env.clear();
  p->accept(this);
  return;
}

void ImpInterpreter::visit(Program* p) {
  p->body->accept(this);
  return;
}

void ImpInterpreter::visit(Body* b) {
  env.add_level(); 
  b->var_decs->accept(this);
  b->slist->accept(this);
  env.remove_level();  
  return;
}


void ImpInterpreter::visit(VarDecList* decs) {
  list<VarDec*>::iterator it;
  for (it = decs->vdlist.begin(); it != decs->vdlist.end(); ++it) {
    (*it)->accept(this);
  }  
  return;
}

void ImpInterpreter::visit(VarDec* vd) {
  list<string>::iterator it;
  ImpValue v;
  ImpType tt = ImpValue::get_basic_type(vd->type);
  if (tt == NOTYPE) {
    cout << "Tipo invalido: " << vd->type << endl;
    exit(0);
  }
  v.set_default_value(tt);
  for (it = vd->vars.begin(); it != vd->vars.end(); ++it) {
    env.add_var(*it,v);
  }   
  return;
}

void ImpInterpreter::visit(StatementList* s) {
  list<Stm*>::iterator it;
  for (it = s->slist.begin(); it != s->slist.end(); ++it) {
    (*it)->accept(this);
  }
  return;
}

void ImpInterpreter::visit(AssignStatement* s) {
  ImpValue v = s->rhs->accept(this);
  if (!env.check(s->id)) {
    cout << "Variable " << s->id << " undefined" << endl;
    exit(0);
  }
  ImpValue lhs = env.lookup(s->id);
  if (lhs.type != v.type) {
    cout << "Type Error en Assign: Tipos de variable " << s->id;
    cout << " y RHS no coinciden" << endl;
    exit(0);
  }
  env.update(s->id, v);
  return;
}

void ImpInterpreter::visit(PrintStatement* s) {
  ImpValue v = s->e->accept(this);
  cout << v << endl;
  return;
}

void ImpInterpreter::visit(IfStatement* s) {
  ImpValue v = s->cond->accept(this);
  if (v.type != TBOOL) {
    cout << "Type error en If: esperaba bool en condicional" << endl;
    exit(0);
  }	      
  if (v.bool_value) {
    s->tbody->accept(this);
  } else {
    if (s->fbody != NULL)
      s->fbody->accept(this);
  }
  return;
}

void ImpInterpreter::visit(WhileStatement* s) {
  ImpValue v = s->cond->accept(this);
  if (v.type != TBOOL) {
    cout << "Type error en While: esperaba bool en condicional" << endl;
    exit(0);
  }	        
 while (v.bool_value) {
    s->body->accept(this);
    v = s->cond->accept(this);
  }
 return;
}

ImpValue ImpInterpreter::visit(BinaryExp* e) {
  ImpValue result;
  ImpValue v1 = e->left->accept(this);
  ImpValue v2 = e->right->accept(this);
  if (v1.type != TINT || v2.type != TINT) {
    cout << "Error de tipos: operandos en operacion binaria tienen que ser enteros" << endl;
    exit(0);
  }
  int iv, iv1, iv2;
  bool bv;
  ImpType type = NOTYPE;
  iv1 = v1.int_value;
  iv2 = v2.int_value;
  switch(e->op) {
  case PLUS: iv = iv1+iv2; type = TINT; break;
  case MINUS: iv = iv1-iv2; type = TINT; break;
  case MULT: iv = iv1 * iv2; type = TINT; break;
  case DIV: iv = iv1 / iv2; type = TINT; break;
  case EXP:
    iv = 1;
    while (iv2 > 0) { iv *= iv1; iv2--; }
    type = TINT;
    break;
  case LT: bv = (iv1 < iv2) ? 1 : 0; type = TBOOL; break;
  case LTEQ: bv = (iv1 <= iv2) ? 1: 0; type = TBOOL; break;
  case EQ: bv = (iv1 == iv2) ? 1 : 0; type = TBOOL; break;
  }
  if (type == TINT) result.int_value = iv;
  else result.bool_value = bv;
  result.type = type;
  return result;
}

ImpValue ImpInterpreter::visit(NumberExp* e) {
  ImpValue v;
  v.set_default_value(TINT);
  v.int_value = e->value;
  return v;
}

ImpValue ImpInterpreter::visit(IdExp* e) {
  if (env.check(e->id))
    return env.lookup(e->id);
  else {
    cout << "Variable indefinida: " << e->id << endl;
    exit(0);
  }
}

ImpValue ImpInterpreter::visit(ParenthExp* ep) {
  return ep->e->accept(this);
}

ImpValue ImpInterpreter::visit(CondExp* e) {
  ImpValue v = e->cond->accept(this);
  if (v.type != TBOOL) {
    cout << "Type error en ifexp: esperaba bool en condicional" << endl;
    exit(0);
  }
  if (v.bool_value == 0)
    return e->efalse->accept(this);
  else
    return e->etrue->accept(this);
}
