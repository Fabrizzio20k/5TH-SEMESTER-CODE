#ifndef EXP_AST
#define EXP_AST

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>

using namespace std;

enum BinaryOp { PLUS, MINUS, MULT, DIV, EXP};

class Exp {
public:
  virtual void print() = 0;
  virtual int eval() = 0;
  static string binopToString(BinaryOp op);
  virtual ~Exp() = 0;
};

class BinaryExp : public Exp {
public:
  Exp *left, *right;
  BinaryOp op;
  BinaryExp(Exp* l, Exp* r, BinaryOp op);
  void print();
  int eval();
  ~BinaryExp();
};

class NumberExp : public Exp {
public:
  int value;
  NumberExp(int v);
  void print();
  int eval();
  ~NumberExp();
};

class ParenthExp : public Exp {
public:
  Exp *e;
  ParenthExp(Exp *e);
  void print();
  int eval();
  ~ParenthExp();
};



#endif
