#include "imp_typechecker.hh"

ImpTypeChecker::ImpTypeChecker()
    : inttype(), booltype(), voidtype(), maintype() {
  inttype.set_basic_type("int");
  booltype.set_basic_type("bool");
  voidtype.set_basic_type("void");
  // maintype
  list<string> noparams;
  maintype.set_fun_type(noparams, "void");
}

void ImpTypeChecker::typecheck(Program *p) {
  env.clear();
  p->accept(this);
  return;
}

void ImpTypeChecker::visit(Program *p) {
  env.add_level();
  p->var_decs->accept(this);
  p->fun_decs->accept(this);
  if (!env.check("main")) {
    cout << "La funcion main no fue definida" << endl;
    exit(0);
  }
  ImpType main_tipo;
  main_tipo = env.lookup("main");
  if (!maintype.match(main_tipo)) {
    cout << "La funcion main no es de tipo void" << endl;
    exit(0);
  }
  env.remove_level();
  cout << "EXITO";
  return;
}

void ImpTypeChecker::visit(Body *b) {
  b->var_decs->accept(this);
  b->slist->accept(this);
  return;
}

void ImpTypeChecker::visit(VarDecList *decs) {
  list<VarDec *>::iterator it;
  for (it = decs->vdlist.begin(); it != decs->vdlist.end(); ++it) {
    (*it)->accept(this);
  }
  return;
}

void ImpTypeChecker::add_fundec(FunDec *fd) {
  ImpType funcion;
  if (!funcion.set_fun_type(fd->types, fd->rtype)) {
    cout << "Tipos definidos en la funcion no valido" << endl;
    exit(0);
  }
  env.add_var(fd->fname, funcion);
}

void ImpTypeChecker::visit(FunDecList *s) {
  list<FunDec *>::iterator it;
  for (it = s->fdlist.begin(); it != s->fdlist.end(); ++it) {
    (*it)->accept(this);
    add_fundec(*it);
  }
  return;
}

void ImpTypeChecker::visit(FunDec *fd) {
  list<string>::iterator it, vit;
  env.add_level();
  for (it = fd->types.begin(), vit = fd->vars.begin(); it != fd->types.end();
       ++it, ++vit) {
    ImpType tipo;
    tipo.set_basic_type(*it);
    env.add_var(*vit, tipo);
  }
  ImpType Tipofuncion;
  Tipofuncion.set_basic_type(fd->rtype);
  env.add_var("Retorno", Tipofuncion);
  fd->body->accept(this);
  env.remove_level();
  return;
}

void ImpTypeChecker::visit(VarDec *vd) {
  ImpType type;
  type.set_basic_type(vd->type);
  if (type.ttype == ImpType::NOTYPE || type.ttype == ImpType::VOID) {
    cout << "Tipo invalido: " << vd->type << endl;
    exit(0);
  }
  list<string>::iterator it;
  for (it = vd->vars.begin(); it != vd->vars.end(); ++it) {
    env.add_var(*it, type);
  }
  return;
}

void ImpTypeChecker::visit(StatementList *s) {
  list<Stm *>::iterator it;
  for (it = s->slist.begin(); it != s->slist.end(); ++it) {
    (*it)->accept(this);
  }
  return;
}

void ImpTypeChecker::visit(AssignStatement *s) {
  ImpType type = s->rhs->accept(this);
  if (!env.check(s->id)) {
    cout << "Variable " << s->id << " undefined" << endl;
    exit(0);
  }
  ImpType var_type = env.lookup(s->id);
  if (!type.match(var_type)) {
    cout << "Tipo incorrecto en Assign a " << s->id << endl;
  }
  return;
}

void ImpTypeChecker::visit(PrintStatement *s) {
  s->e->accept(this);
  return;
}

void ImpTypeChecker::visit(IfStatement *s) {
  if (!s->cond->accept(this).match(booltype)) {
    cout << "Expresion conditional en IF debe de ser bool" << endl;
    exit(0);
  }
  s->tbody->accept(this);
  if (s->fbody != NULL)
    s->fbody->accept(this);
  return;
}

void ImpTypeChecker::visit(WhileStatement *s) {
  if (!s->cond->accept(this).match(booltype)) {
    cout << "Expresion conditional en IF debe de ser bool" << endl;
    exit(0);
  }
  s->body->accept(this);
  return;
}

void ImpTypeChecker::visit(ReturnStatement *s) {
  if (s->e != NULL) {
    ImpType retorno_expresion;
    retorno_expresion = s->e->accept(this);
    ImpType tipo;
    tipo = env.lookup("Retorno");
    if (!tipo.match(retorno_expresion)) {
      cout << "El tipo de retorno de la funcion es incorrecto." << endl;
      exit(0);
    };
  }
  return;
}

ImpType ImpTypeChecker::visit(BinaryExp *e) {
  ImpType t1 = e->left->accept(this);
  ImpType t2 = e->right->accept(this);
  if (!t1.match(inttype) || !t2.match(inttype)) {
    cout << "Tipos en BinExp deben de ser int" << endl;
    exit(0);
  }
  ImpType result;
  switch (e->op) {
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

  return result;
}

ImpType ImpTypeChecker::visit(NumberExp *e) { return inttype; }

ImpType ImpTypeChecker::visit(TrueFalseExp *e) { return booltype; }

ImpType ImpTypeChecker::visit(IdExp *e) {
  if (env.check(e->id))
    return env.lookup(e->id);
  else {
    cout << "Variable indefinida: " << e->id << endl;
    exit(0);
  }
}

ImpType ImpTypeChecker::visit(ParenthExp *ep) { return ep->e->accept(this); }

ImpType ImpTypeChecker::visit(CondExp *e) {
  if (!e->cond->accept(this).match(booltype)) {
    cout << "Tipo en ifexp debe de ser bool" << endl;
    exit(0);
  }
  ImpType ttype = e->etrue->accept(this);
  if (!ttype.match(e->efalse->accept(this))) {
    cout << "Tipos en ifexp deben de ser iguales" << endl;
    exit(0);
  }
  return ttype;
}

ImpType ImpTypeChecker::visit(FCallExp *e) {
  /*
  cout << e->fname << "(";
  list<Exp*>::iterator it;
  bool first = true;
  for (it = e->args.begin(); it != e->args.end(); ++it) {
    if (!first) cout << ",";
    first = false;
    (*it)->accept(this);

  }
  cout << ')';
  */
  return inttype;
}
