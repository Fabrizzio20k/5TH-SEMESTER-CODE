#ifndef IMP_INTERPRETER
#define IMP_INTERPRETER

#include <unordered_map>
#include "imp.hh"
#include "imp_visitor.hh"
#include "environment.hh"

using namespace std;

class ImpInterpreter : public ImpVisitor {
private:
  Environment env;
public:
    int interpret(Program*);
    int visit(Program*);
    int visit(AssignStatement*);
    int visit(PrintStatement*);
    int visit(Cadena* e);
    int visit(BinaryExp* e);
    int visit(NumberExp* e);
    int visit(IdExp* e);
    int visit(ParenthExp* e);
};


#endif

