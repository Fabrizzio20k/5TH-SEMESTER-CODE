#ifndef IMP_AST
#define IMP_AST

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>

#include <list>

using namespace std;

class ImpASTVisitor;

enum BinaryOp { PLUS, MINUS, MULT, DIV, EXP};

  
class Exp {
public:
  virtual int accept(ImpASTVisitor* v) = 0;
  static string binopToString(BinaryOp op);
  virtual ~Exp() = 0;
};

class BinaryExp : public Exp {
public:
  Exp *left, *right;
  BinaryOp op;
  BinaryExp(Exp* l, Exp* r, BinaryOp op);
  int accept(ImpASTVisitor* v);
  ~BinaryExp();
};

class NumberExp : public Exp {
public:
  int value;
  NumberExp(int v);
  int accept(ImpASTVisitor* v);
  ~NumberExp();
};

class IdExp : public Exp {
public:
  string id;
  IdExp(string id);
  int accept(ImpASTVisitor* v);
  ~IdExp();
};

class ParenthExp : public Exp {
public:
  Exp *e;
  ParenthExp(Exp *e);
  int accept(ImpASTVisitor* v);
  ~ParenthExp();
};

class Stm {
public:
  virtual int accept(ImpASTVisitor* v) = 0;
  virtual ~Stm() = 0;
};


class AssignStatement : public Stm {
public:
  string id;
  Exp* rhs;  
  AssignStatement(string id, Exp* e);
  int accept(ImpASTVisitor* v);
  ~AssignStatement();
};

class PrintStatement : public Stm {
public:
  Exp* e;  
  PrintStatement(Exp* e);
  int accept(ImpASTVisitor* v);
  ~PrintStatement();
};



class Program {
public:
  list<Stm*> slist;
  Program();
  void add(Stm* s);
  int accept(ImpASTVisitor* v);
  ~Program();
};



#endif

