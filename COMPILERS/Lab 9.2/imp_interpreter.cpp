#include "imp_interpreter.hh"
#include "imp_value.hh"

/* *************** Fin manejo del estado del programa ****** */

void ImpInterpreter::interpret(Program *p)
{
  env.clear();
  p->accept(this);
  return;
}

void ImpInterpreter::visit(Program *p)
{
  p->body->accept(this);
  return;
}

void ImpInterpreter::visit(Body *b)
{
  env.add_level();
  b->var_decs->accept(this);
  b->slist->accept(this);
  env.remove_level();
  return;
}

void ImpInterpreter::visit(VarDecList *decs)
{
  list<VarDec *>::iterator it;
  for (it = decs->vdlist.begin(); it != decs->vdlist.end(); ++it)
  {
    (*it)->accept(this);
  }
  return;
}

void ImpInterpreter::visit(VarDec *vd)
{
  list<string>::iterator it;
  ImpValue val;
  ImpType ty = val.get_basic_type(vd->type);

  val.set_default_value(ty);

  if (ty == ImpType::NOTYPE)
  {
    cout << "Error: tipo de variable no reconocido" << endl;
    exit(0);
  }

  for (it = vd->vars.begin(); it != vd->vars.end(); ++it)
  {
    env.add_var(*it, val);
  }
  return;
}

void ImpInterpreter::visit(StatementList *s)
{
  list<Stm *>::iterator it;
  for (it = s->slist.begin(); it != s->slist.end(); ++it)
  {
    (*it)->accept(this);
  }
  return;
}

void ImpInterpreter::visit(AssignStatement *s)
{
  ImpValue val = s->rhs->accept(this);

  if (!env.check(s->id))
  {
    cout << "Variable " << s->id << " undefined" << endl;
    exit(0);
  }

  ImpValue var = env.lookup(s->id);

  if (val.type != var.type)
  {
    cout << "Tipos de variables diferentes" << endl;
    exit(0);
  }

  env.update(s->id, val);
  return;
}

void ImpInterpreter::visit(PrintStatement *s)
{
  ImpValue v = s->e->accept(this);
  cout << v << endl;
  return;
}

void ImpInterpreter::visit(IfStatement *s)
{
  if (s->cond->accept(this).type != ImpType::TBOOL)
  {
    cout << "Error: la condicion no es de tipo bool" << endl;
    exit(0);
  }

  if (s->cond->accept(this).bool_value)
  {
    s->tbody->accept(this);
  }
  else
  {
    if (s->fbody != NULL)
      s->fbody->accept(this);
  }
  return;
}

void ImpInterpreter::visit(WhileStatement *s)
{
  if (s->cond->accept(this).type != ImpType::TBOOL)
  {
    cout << "Error: la condicion no es de tipo bool" << endl;
    exit(0);
  }

  while (s->cond->accept(this).bool_value)
  {
    s->body->accept(this);
  }
  return;
}

// ################################################

ImpValue ImpInterpreter::visit(BinaryExp *e)
{
  int v1 = e->left->accept(this).int_value;
  int v2 = e->right->accept(this).int_value;
  ImpValue result;
  switch (e->op)
  {
  case PLUS:
    result.set_default_value(ImpType::TINT);
    result.int_value = v1 + v2;
    break;
  case MINUS:
    result.set_default_value(ImpType::TINT);
    result.int_value = v1 - v2;
    break;
  case MULT:
    result.set_default_value(ImpType::TINT);
    result.int_value = v1 * v2;
    break;
  case DIV:
    result.set_default_value(ImpType::TINT);
    result.int_value = v1 / v2;
    break;
  case EXP:
    result.set_default_value(ImpType::TINT);
    result.int_value = 1;
    while (v2 > 0)
    {
      result.int_value *= v1;
      v2--;
    }
    break;
  case LT:
    result.set_default_value(ImpType::TBOOL);
    result.bool_value = (v1 < v2) ? true : false;
    break;
  case LTEQ:
    result.set_default_value(ImpType::TBOOL);
    result.bool_value = (v1 <= v2) ? true : false;
    break;
  case EQ:
    result.set_default_value(ImpType::TBOOL);
    result.bool_value = (v1 == v2) ? true : false;
    break;
  }

  return result;
}

ImpValue ImpInterpreter::visit(NumberExp *e)
{
  ImpValue v;
  v.set_default_value(ImpType::TINT);
  v.int_value = e->value;
  return v;
}

ImpValue ImpInterpreter::visit(IdExp *e)
{
  if (env.check(e->id))
    return env.lookup(e->id);
  else
  {
    cout << "Variable indefinida: " << e->id << endl;
    exit(0);
  }
}

ImpValue ImpInterpreter::visit(ParenthExp *ep)
{
  return ep->e->accept(this);
}

ImpValue ImpInterpreter::visit(CondExp *e)
{
  if (e->cond->accept(this).type != ImpType::TBOOL)
  {
    cout << "Error: la condicion no es de tipo bool" << endl;
    exit(0);
  }

  if (e->cond->accept(this).bool_value == false)
    return e->efalse->accept(this);
  else
    return e->etrue->accept(this);
}
