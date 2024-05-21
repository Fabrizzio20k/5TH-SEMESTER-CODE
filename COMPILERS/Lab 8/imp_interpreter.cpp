#include "imp_interpreter.hh"


int ImpInterpreter::interpret(Program *p) {
    p->accept(this);
    return 0;
}

void ImpInterpreter::visit(Program *p) {
    p->slist->accept(this);
    return;
}

void ImpInterpreter::visit(StatementList *s) {
    list<Stm *>::iterator it;
    for (it = s->slist.begin(); it != s->slist.end(); ++it) {
        (*it)->accept(this);
    }
    return;
}

void ImpInterpreter::visit(AssignStatement *s) {
    int v = s->rhs->accept(this);
    env.update(s->id, v);
    return;
}

void ImpInterpreter::visit(PrintStatement *s) {
    int v = s->e->accept(this);
    cout << v << endl;
    return;
}

int ImpInterpreter::visit(BinaryExp *e) {
    int v1 = e->left->accept(this);
    int v2 = e->right->accept(this);
    int result = 0;
    switch (e->op) {
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
        case LT:
            result = v1 < v2;
            break;
        case LTEQ:
            result = v1 <= v2;
            break;
        case EQ:
            result = v1 == v2;
            break;
        case EXP:
            result = 1;
            while (v2 > 0) {
                result *= v1;
                v2--;
            }
            break;
    }
    return result;
}

int ImpInterpreter::visit(NumberExp *e) {
    return e->value;
}

int ImpInterpreter::visit(IdExp *e) {
    if (env.check(e->id))
        return env.lookup(e->id);
    else {
        cout << "Variable indefinida: " << e->id << endl;
        exit(0);
    }
    return 0;
}

int ImpInterpreter::visit(ParenthExp *ep) {
    return ep->e->accept(this);
}
