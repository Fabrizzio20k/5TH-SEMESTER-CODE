#include "imp_typechecker.hh"

void ImpTypeChecker::sp_incr(int n) {
  sp += n;
  if (sp > max_sp) max_sp = sp;
}

void ImpTypeChecker::sp_decr(int n) {
  sp -= n;
  if (sp < 0) {
    cout << "stack less than 0" << endl;
    exit(0);
  }
}

ImpTypeChecker::ImpTypeChecker():inttype(),booltype() {
  inttype.set_basic_type("int");
  booltype.set_basic_type("bool");
  sp  = max_sp = 0;
}

void ImpTypeChecker::typecheck(Program* p) {
  env.clear();
  p->accept(this);
  cout << "Max stack size: " << max_sp << endl;
  return;
}

void ImpTypeChecker::visit(Program* p) {
  p->body->accept(this);
  return;
}

void ImpTypeChecker::visit(Body* b) {
  env.add_level();
  b->var_decs->accept(this);
  b->slist->accept(this);
  env.remove_level();  
  return;
}

void ImpTypeChecker::visit(VarDecList* decs) {
  list<VarDec*>::iterator it;
  for (it = decs->vdlist.begin(); it != decs->vdlist.end(); ++it) {
    (*it)->accept(this);
  }  
  return;
}

void ImpTypeChecker::visit(VarDec* vd) {
  ImpType type;
  type.set_basic_type(vd->type);
  if (type.ttype==ImpType::NOTYPE || type.ttype==ImpType::VOID) {
    cout << "Tipo invalido: " << vd->type << endl;
    exit(0);
  }
  list<string>::iterator it;
  for (it = vd->vars.begin(); it != vd->vars.end(); ++it) {
    env.add_var(*it, type);
  }   
  return;
}


void ImpTypeChecker::visit(StatementList* s) {
  list<Stm*>::iterator it;
  for (it = s->slist.begin(); it != s->slist.end(); ++it) {
    (*it)->accept(this);
  }
  return;
}

void ImpTypeChecker::visit(AssignStatement* s) {
  ImpType type = s->rhs->accept(this);
  if (!env.check(s->id)) {
    cout << "Variable " << s->id << " undefined" << endl;
    exit(0);
  }
  sp_decr(1);
  ImpType var_type = env.lookup(s->id);  
  if (!type.match(var_type)) {
    cout << "Tipo incorrecto en Assign a " << s->id << endl;
    exit(0);
  }
  return;
}

void ImpTypeChecker::visit(PrintStatement* s) {
  s->e->accept(this);
  sp_decr(1);
  return;
}

void ImpTypeChecker::visit(IfStatement* s) {
  s->cond->accept(this);
  sp_decr(1);
  s->tbody->accept(this);
  if (s->fbody != NULL)
    s->fbody->accept(this);
  return;
}

void ImpTypeChecker::visit(WhileStatement* s) {
  s->cond->accept(this);
  sp_decr(1);
  s->body->accept(this);
 return;
}

ImpType ImpTypeChecker::visit(BinaryExp* e) {
  ImpType t1 = e->left->accept(this);
  ImpType t2 = e->right->accept(this);
  if (!t1.match(inttype) || !t2.match(inttype)) {
    cout << "Tipos en BinExp deben de ser int" << endl;
    exit(0);
  }
  ImpType result;
  switch(e->op) {
  case PLUS: 
  case MINUS:
  case MULT:
  case DIV:
  case EXP:
    result = inttype;
    break;
  case LT: 
  case LTEQ:
  case EQ:
    result = booltype;
    break;
  }
  sp_decr(1);
  return result;
}

ImpType ImpTypeChecker::visit(UnaryExp* e) {
  ImpType etype = e->e->accept(this);
  ImpType rtype;
  if (e->op == NEG) rtype.set_basic_type("int");
  else rtype.set_basic_type("bool");
  if (!etype.match(rtype)) {
    cout << "Tipo incorrecto para operacion binaria" << endl;
    exit(0);
  }
  return rtype;
}

ImpType ImpTypeChecker::visit(NumberExp* e) {
  sp_incr(1);
  return inttype;
}

ImpType ImpTypeChecker::visit(IdExp* e) {
  sp_incr(1);
  if (env.check(e->id))
    return env.lookup(e->id);
  else {
    cout << "Variable indefinida: " << e->id << endl;
    exit(0);
  }
}

ImpType ImpTypeChecker::visit(ParenthExp* ep) {
  return ep->e->accept(this);
}

ImpType ImpTypeChecker::visit(CondExp* e) {
  ImpType booltype;
  booltype.set_basic_type("bool");
  if (!e->cond->accept(this).match(booltype)) {
    cout << "Tipo en ifexp debe de ser bool" << endl;
    exit(0);
  }
  sp_decr(1);
  int sp_start = sp;
  ImpType ttype =  e->etrue->accept(this);
  sp = sp_start;
  if (!ttype.match(e->efalse->accept(this))) {
    cout << "Tipos en ifexp deben de ser iguales" << endl;
    exit(0);
  }
  return ttype;
}


/*
while 0 < x do accum = accum+x; x=x-1 endwhile;
 */
