#ifndef IMP_INTERPRETER
#define IMP_INTERPRETER

#include <unordered_map>
#include "imp.hh"
#include "imp_ast_visitor.hh"

class ImpInterpreter : public ImpASTVisitor {
private:
  unordered_map<string, int> memoria;
  void memoria_update(string x, int v);
  bool memoria_check(string x);
  int  memoria_lookup(string x);  
public:
  int interpret(Program*);
  // visit para 3 clases

  
  int visit(BinaryExp* e);
  int visit(NumberExp* e);
  int visit(IdExp* e);
  int visit(ParenthExp* e);
};


#endif

