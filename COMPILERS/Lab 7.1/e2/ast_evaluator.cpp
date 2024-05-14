#include "ast_evaluator.hh"
#include <cmath>

int ASTEvaluator::eval(Exp *e) {
    return e->accept(this);
}

int ASTEvaluator::visit(BinaryExp *e) {
    int left = e->left->accept(this);
    int right = e->right->accept(this);
    int result;
    switch (e->op) {
        case PLUS:
            result = left + right;
            break;
        case MINUS:
            result = left - right;
            break;
        case MULT:
            result = left * right;
            break;
        case DIV:
            result = left / right;
            break;
        case EXP:
            result = pow(left, right);
            break;

    }
    return result;
}

int ASTEvaluator::visit(NumberExp *e) {
    return e->value;
}

int ASTEvaluator::visit(ParenthExp *ep) {
    return ep->e->accept(this);
}
