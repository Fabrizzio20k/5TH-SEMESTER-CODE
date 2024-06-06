#ifndef SVMHH
#define SVMHH

#define MEM_SIZE 5000

#include <string>
#include <list>
#include <stack>

#include <vector>
#include <unordered_map>

using namespace std;

class Instruction {
public:
  enum IType { IPUSH=0, IPOP, IDUP, ISWAP, IADD, ISUB, IMUL, IDIV, IGOTO, IEQ, IGT, IGE, ILT, ILE, ISKIP, ISTORE, ILOAD, IPRINT, IJMPZ, IJMPN, IAND, IOR, INEG, INOT , IHALT, ICALL, IMARK, ISTORER, ILOADR, IPUSHA, IENTER, IALLOC, IRETURN };
  string label, jmplabel;
  IType type;
  bool hasarg;
  int argint; // could be label or int argument
  Instruction(string l, IType itype);
  Instruction(string l, IType itype, int arg);
  Instruction(string l, IType itype, string arg);  
  //static InstrType convertKeywordType(Token::KeywordType ktype);
};


class SVM {
private:

  int sp, pc, fp, ep, hp;
  int stack[MEM_SIZE];
  int max_stack;

  vector<Instruction*> instructions;
  unordered_map<string,int> labels;

  bool execute(Instruction* s);
  void svm_error(string msg);
  void stack_pop();
  void stack_push(int v);
  int stack_top();
  bool stack_empty();
  void memory_write(int,int);
  int memory_read(int);
  bool check_memory(int);
  void execute_mark();
  void execute_call();
  void execute_enter(int m);
  void execute_alloc(int n);
  void execute_return(int n);
public:
  SVM(list<Instruction*>&  sl);
  void execute();
  void print_stack();
  void print();
  int top();
};


#endif
