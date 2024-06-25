#include "imp_typechecker.hh"

ImpTypeChecker::ImpTypeChecker() : inttype(), booltype(), voidtype(), maintype()
{
  // basic types
  inttype.set_basic_type("int");
  booltype.set_basic_type("bool");
  voidtype.set_basic_type("void");

  // maintype
  list<string> noparams;
  maintype.set_fun_type(noparams, "void");
}

void ImpTypeChecker::typecheck(Program *p)
{
  env.clear();
  p->accept(this);
  return;
}

void ImpTypeChecker::visit(Program *p)
{
  env.add_level();
  p->var_decs->accept(this);
  p->fun_decs->accept(this);

  // check main
  if (!env.check("main"))
  {
    cout << "Funcion main no definida" << endl;
    exit(0);
  }

  ImpType main_type = env.lookup("main");
  if (!main_type.match(maintype))
  {
    cout << "Main debe de ser void" << endl;
    exit(0);
  }

  env.remove_level();
  return;
}

void ImpTypeChecker::visit(Body *b)
{
  b->var_decs->accept(this);
  b->slist->accept(this);
  return;
}

void ImpTypeChecker::visit(VarDecList *decs)
{
  list<VarDec *>::iterator it;
  for (it = decs->vdlist.begin(); it != decs->vdlist.end(); ++it)
  {
    (*it)->accept(this);
  }
  return;
}

// Por implementar
void ImpTypeChecker::add_fundec(FunDec *fd)
{
  ImpType funcion;
  if (!funcion.set_fun_type(fd->types, fd->rtype))
  {
    cout << "Tipo de retorno invalido: " << fd->rtype << endl;
  }
  env.add_var(fd->fname, funcion);
  return;
}

void ImpTypeChecker::visit(FunDecList *s)
{
  list<FunDec *>::iterator it;
  // agregar loop que llama a add_fun_dec

  for (it = s->fdlist.begin(); it != s->fdlist.end(); ++it)
  {
    add_fundec(*it);
    cout << endl;
  }

  for (it = s->fdlist.begin(); it != s->fdlist.end(); ++it)
  {
    (*it)->accept(this);
  }

  return;
}

void ImpTypeChecker::visit(FunDec *fd)
{
  env.add_level();

  list<string>::iterator it, vit;
  for (it = fd->types.begin(), vit = fd->vars.begin();
       it != fd->types.end(); ++it, ++vit)
  {
    // first = false;
    ImpType type;
    type.set_basic_type(*it);

    if (type.ttype == ImpType::NOTYPE || type.ttype == ImpType::VOID)
    {
      cout << "Tipo invalido: " << *it << endl;
      exit(0);
    }

    env.add_var(*vit, type);

    cout << *it << " " << *vit;
  }
  ImpType funtype;
  funtype.set_basic_type(fd->rtype);
  env.add_var("return", funtype);

  fd->body->accept(this);
  env.remove_level();
  return;
}

void ImpTypeChecker::visit(VarDec *vd)
{
  ImpType type;
  type.set_basic_type(vd->type);
  if (type.ttype == ImpType::NOTYPE || type.ttype == ImpType::VOID)
  {
    cout << "Tipo invalido: " << vd->type << endl;
    exit(0);
  }
  list<string>::iterator it;
  for (it = vd->vars.begin(); it != vd->vars.end(); ++it)
  {
    env.add_var(*it, type);
  }
  return;
}

void ImpTypeChecker::visit(StatementList *s)
{
  list<Stm *>::iterator it;
  for (it = s->slist.begin(); it != s->slist.end(); ++it)
  {
    (*it)->accept(this);
  }
  return;
}

void ImpTypeChecker::visit(AssignStatement *s)
{
  ImpType type = s->rhs->accept(this);
  if (!env.check(s->id))
  {
    cout << "Variable " << s->id << " undefined" << endl;
    exit(0);
  }
  ImpType var_type = env.lookup(s->id);
  if (!type.match(var_type))
  {
    cout << "Tipo incorrecto en Assign a " << s->id << endl;
  }
  return;
}

void ImpTypeChecker::visit(PrintStatement *s)
{
  s->e->accept(this);
  return;
}

void ImpTypeChecker::visit(IfStatement *s)
{
  if (!s->cond->accept(this).match(booltype))
  {
    cout << "Expresion conditional en IF debe de ser bool" << endl;
    exit(0);
  }
  s->tbody->accept(this);
  if (s->fbody != NULL)
    s->fbody->accept(this);
  return;
}

void ImpTypeChecker::visit(WhileStatement *s)
{
  if (!s->cond->accept(this).match(booltype))
  {
    cout << "Expresion conditional en IF debe de ser bool" << endl;
    exit(0);
  }
  s->body->accept(this);
  return;
}

void ImpTypeChecker::visit(ReturnStatement *s)
{
  ImpType etype;
  if (s->e != NULL)
  {
    etype = s->e->accept(this);
  }
  else
  {
    etype = voidtype;
  }

  ImpType rtype = env.lookup("return");
  if (!etype.match(rtype))
  {
    cout << "Tipo de retorno incorrecto" << endl;
    exit(0);
  }
  return;
}

ImpType ImpTypeChecker::visit(BinaryExp *e)
{
  ImpType t1 = e->left->accept(this);
  ImpType t2 = e->right->accept(this);
  if (!t1.match(inttype) || !t2.match(inttype))
  {
    cout << "Tipos en BinExp deben de ser int" << endl;
    exit(0);
  }
  ImpType result;
  switch (e->op)
  {
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

ImpType ImpTypeChecker::visit(NumberExp *e)
{
  return inttype;
}

ImpType ImpTypeChecker::visit(TrueFalseExp *e)
{
  return booltype;
}

ImpType ImpTypeChecker::visit(IdExp *e)
{
  if (env.check(e->id))
    return env.lookup(e->id);
  else
  {
    cout << "Variable indefinida: " << e->id << endl;
    exit(0);
  }
}

ImpType ImpTypeChecker::visit(ParenthExp *ep)
{
  return ep->e->accept(this);
}

ImpType ImpTypeChecker::visit(CondExp *e)
{
  if (!e->cond->accept(this).match(booltype))
  {
    cout << "Tipo en ifexp debe de ser bool" << endl;
    exit(0);
  }
  ImpType ttype = e->etrue->accept(this);
  if (!ttype.match(e->efalse->accept(this)))
  {
    cout << "Tipos en ifexp deben de ser iguales" << endl;
    exit(0);
  }
  return ttype;
}

ImpType ImpTypeChecker::visit(FCallExp *e)
{
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
  // return inttype;

  /*
  • Verificar que fname existe y es una función declarada (ImpType::FUN).
• Verificar que el número de argumentos es el mismo que el número de parámetros.
• Verificar que el tipo de los argumentos corresponde al tipo de los parámetros.
• Retornar el tipo de retorno de la funcion

  */

  if (!env.check(e->fname))
  {
    cout << "Funcion " << e->fname << " no definida" << endl;
    exit(0);
  }

  ImpType funtype = env.lookup(e->fname);
  if (funtype.ttype != ImpType::FUN)
  {
    cout << "Variable " << e->fname << " no es una funcion" << endl;
    exit(0);
  }

  if (funtype.types.size() != e->args.size())
  {
    cout << "Numero de argumentos incorrecto en llamada a " << e->fname << endl;
    exit(0);
  }
}
