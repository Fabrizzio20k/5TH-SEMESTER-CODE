#include "imp_interpreter.hh"

void ImpInterpreter::interpret(Program* p) {
  env.clear();
  p->accept(this);
  return;
}

// modificar
void ImpInterpreter::visit(Program* p) {
  env.add_level();
  fdecs.add_level();
  p->var_decs->accept(this);
  p->fun_decs->accept(this);
  if (!fdecs.check("main")) {
    cout << "Error: No se encontro funcion main" << endl;
    exit(0);
  }
  FunDec* main_dec = fdecs.lookup("main");
  retcall = false;
  main_dec->body->accept(this);
  if (!retcall) {
    cout << "Error: Funcion main no ejecuto RETURN" << endl;
    exit(0);
  }
  fdecs.remove_level();
  env.remove_level();
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

void ImpInterpreter::visit(FunDecList* s) {
  list<FunDec*>::iterator it;
  for (it = s->fdlist.begin(); it != s->fdlist.end(); ++it) {
    (*it)->accept(this);
  }
  return;
} 

void ImpInterpreter::visit(VarDec* vd) {
  list<string>::iterator it;
  ImpValue v;
  ImpVType tt = ImpValue::get_basic_type(vd->type);
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

void ImpInterpreter::visit(FunDec* fd) {
  fdecs.add_var(fd->fname, fd);
  return;
}

void ImpInterpreter::visit(StatementList* s) {
  list<Stm*>::iterator it;
  for (it = s->slist.begin(); it != s->slist.end(); ++it) {
    (*it)->accept(this);
    if (retcall) break;
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

void ImpInterpreter::visit(ReturnStatement* s) {
  if (s->e != NULL)
    retval = s->e->accept(this);
  retcall = true;
  return;
}

// Expressions

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
  ImpVType type = NOTYPE;
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

ImpValue ImpInterpreter::visit(TrueFalseExp* e) {
  ImpValue v;
  v.set_default_value(TBOOL);
  v.bool_value = e->value;
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

ImpValue ImpInterpreter::visit(FCallExp* e) {
  FunDec* fdec = fdecs.lookup(e->fname);
  env.add_level();
  list<Exp*>::iterator it;
  list<string>::iterator varit;
  list<string>::iterator vartype;
  ImpVType tt;
  // comparar longitud
  if (fdec->vars.size() != e->args.size()) {
    cout << "Error: Numero de parametros incorrecto en llamada a " << fdec->fname << endl;
    exit(0);
  }
  for (it = e->args.begin(), varit = fdec->vars.begin(), vartype = fdec->types.begin();
       it != e->args.end(); ++it, ++varit, ++vartype) {
    tt = ImpValue::get_basic_type(*vartype);
    ImpValue v = (*it)->accept(this);
    if (v.type != tt) {
      cout << "Error FCall: Tipos de param y arg no coinciden. Funcion " << fdec->fname << " param " << *varit << endl;
      exit(0);
    }
    env.add_var(*varit, v);
  }
  retcall = false;
  fdec->body->accept(this);
  if (!retcall) {
    cout << "Error: Funcion " << e->fname << " no ejecuto RETURN" << endl;
    exit(0);
  }
  retcall = false;
  env.remove_level();
  // chequear tipo de retorno.
  tt = ImpValue::get_basic_type(fdec->rtype);
  if (tt != retval.type) {
    cout << "Error: Tipo de retorno incorrecto de funcion " << fdec->fname << endl;
    exit(0);
  }
  return retval;
}
