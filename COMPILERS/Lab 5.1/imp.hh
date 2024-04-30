#ifndef IMP_AST
#define IMP_AST

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>

#include <list>
#include <unordered_map>

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

class IdExp : public Exp {
public:
  string id;
  IdExp(string id);
  void print();
  int eval();
  ~IdExp();
};

class ParenthExp : public Exp {
public:
  Exp *e;
  ParenthExp(Exp *e);
  void print();
  int eval();
  ~ParenthExp();
};

class Stm {
public:
  virtual void print()=0;
  virtual void execute()=0;
  virtual ~Stm() = 0;
};


class AssignStatement : public Stm {
private:
  string id;
  Exp* rhs;
public:
  AssignStatement(string id, Exp* e);
  void print();
  void execute();
  ~AssignStatement();
};

class PrintStatement : public Stm {
private:
  Exp* e;
public:
  PrintStatement(Exp* e);
  void print();
  void execute();
  ~PrintStatement();
};



class Program {
private:
  static unordered_map<string, int> memoria;
  list<Stm*> slist;
public:
  Program();
  void add(Stm* s);
  void print();
  void execute();
  ~Program();
  static void memoria_update(string x, int v);
  static bool memoria_check(string x);
  static int  memoria_lookup(string x);

};



#endif

