#ifndef EXP_AST
#define EXP_AST

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>


class ASTVisitor;


using namespace std;

enum BinaryOp { PLUS, MINUS, MULT, DIV, EXP };

class Exp {
public:
  virtual int accept(ASTVisitor* v) = 0;
  static string binopToString(BinaryOp op);
  virtual ~Exp() = 0;
};

class BinaryExp : public Exp {
public:
  Exp *left, *right;
  BinaryOp op;
  BinaryExp(Exp* l, Exp* r, BinaryOp op);
  int accept(ASTVisitor* v); 
  ~BinaryExp();
};

class NumberExp : public Exp {
public:
  int value;
  NumberExp(int v);
  int accept(ASTVisitor* v);
  ~NumberExp();
};

class ParenthExp : public Exp {
public:
  Exp *e;
  ParenthExp(Exp *e);
  int accept(ASTVisitor* v);
  ~ParenthExp();
};



#endif
