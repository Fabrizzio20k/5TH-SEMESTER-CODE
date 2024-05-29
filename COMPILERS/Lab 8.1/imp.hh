#ifndef IMP_AST
#define IMP_AST

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>

#include <list>

using namespace std;

class ImpVisitor;

enum BinaryOp { PLUS, MINUS, MULT, DIV, EXP, LT, LTEQ, EQ };

  
class Exp {
public:
  virtual int accept(ImpVisitor* v) = 0;
  static string binopToString(BinaryOp op);
  virtual ~Exp() = 0;
};

class BinaryExp : public Exp {
public:
  Exp *left, *right;
  BinaryOp op;
  BinaryExp(Exp* l, Exp* r, BinaryOp op);
  int accept(ImpVisitor* v);
  ~BinaryExp();
};

class NumberExp : public Exp {
public:
  int value;
  NumberExp(int v);
  int accept(ImpVisitor* v);
  ~NumberExp();
};

class IdExp : public Exp {
public:
  string id;
  IdExp(string id);
  int accept(ImpVisitor* v);
  ~IdExp();
};

class ParenthExp : public Exp {
public:
  Exp *e;
  ParenthExp(Exp *e);
  int accept(ImpVisitor* v);
  ~ParenthExp();
};

class Stm {
public:
  virtual void accept(ImpVisitor* v) = 0;
  virtual ~Stm() = 0;
};

class StatementList {
public:
  list<Stm*> slist;
  StatementList();
  void add(Stm* s);
  void accept(ImpVisitor* v);
  ~StatementList();
};

class AssignStatement : public Stm {
public:
  string id;
  Exp* rhs;  
  AssignStatement(string id, Exp* e);
  void accept(ImpVisitor* v);
  ~AssignStatement();
};

class PrintStatement : public Stm {
public:
  Exp* e;  
  PrintStatement(Exp* e);
  void accept(ImpVisitor* v);
  ~PrintStatement();
};

class IfStatement : public Stm {
public:
  Exp* cond;
  StatementList *tsl, *fsl;
  IfStatement(Exp* c, StatementList* tsl, StatementList *fsl);
  void accept(ImpVisitor* v);
  ~IfStatement();
};


class Program {
public:
  StatementList* slist;
  Program(StatementList* sl);
  void accept(ImpVisitor* v);
  ~Program();
};


#endif

