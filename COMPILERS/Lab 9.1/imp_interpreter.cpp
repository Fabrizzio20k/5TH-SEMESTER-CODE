#include "imp_interpreter.hh"

/* *************** Fin manejo del estado del programa ****** */

void ImpInterpreter::interpret(Program *p)
{
  env.clear();
  env.add_level();
  // add level
  p->accept(this);
  return;
}

void ImpInterpreter::visit(Program *p)
{
  env.add_level();
  p->body->accept(this);
  return;
}

int ImpInterpreter::visit(Body *b)
{
  b->var_decs->accept(this);
  b->slist->accept(this);
  return 0;
}

int ImpInterpreter::visit(VarDecList *vdl)
{
  list<VarDec *>::iterator it;
  for (it = vdl->vdlist.begin(); it != vdl->vdlist.end(); ++it)
  {
    (*it)->accept(this);
  }
  return 0;
}

int ImpInterpreter::visit(VarDec *vd)
{
  list<string>::iterator it;
  for (it = vd->vars.begin(); it != vd->vars.end(); ++it)
  {
    env.add_var(*it, 0);
  }
  return 0;
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
  int v = s->rhs->accept(this);
  // si la variable no esta en environment, agregarla al nivel
  if (!env.check(s->id))
    env.add_var(s->id, 0);
  env.update(s->id, v);
  return;
}

void ImpInterpreter::visit(PrintStatement *s)
{
  int v = s->e->accept(this);
  cout << v << endl;
  return;
}

void ImpInterpreter::visit(IfStatement *s)
{
  if (s->cond->accept(this))
  {
    s->tsl->accept(this);
  }
  else
  {
    if (s->fsl != NULL)
      s->fsl->accept(this);
  }
  return;
}

void ImpInterpreter::visit(WhileStatement *s)
{
  while (s->cond->accept(this))
  {
    s->sl->accept(this);
  }
  return;
}

int ImpInterpreter::visit(BinaryExp *e)
{
  int v1 = e->left->accept(this);
  int v2 = e->right->accept(this);
  int result = 0;
  switch (e->op)
  {
  case PLUS:
    result = v1 + v2;
    break;
  case MINUS:
    result = v1 - v2;
    break;
  case MULT:
    result = v1 * v2;
    break;
  case DIV:
    result = v1 / v2;
    break;
  case EXP:
    result = 1;
    while (v2 > 0)
    {
      result *= v1;
      v2--;
    }
    break;
  case LT:
    result = (v1 < v2) ? 1 : 0;
    break;
  case LTEQ:
    result = (v1 <= v2) ? 1 : 0;
    break;
  case EQ:
    result = (v1 == v2) ? 1 : 0;
    break;
  }
  return result;
}

int ImpInterpreter::visit(NumberExp *e)
{
  return e->value;
}

int ImpInterpreter::visit(IdExp *e)
{
  if (env.check(e->id))
    return env.lookup(e->id);
  else
  {
    cout << "Variable indefinida: " << e->id << endl;
    exit(0);
  }
}

int ImpInterpreter::visit(ParenthExp *ep)
{
  return ep->e->accept(this);
}

int ImpInterpreter::visit(CondExp *e)
{
  if (e->cond->accept(this) == 0)
    return e->efalse->accept(this);
  else
    return e->etrue->accept(this);
}
