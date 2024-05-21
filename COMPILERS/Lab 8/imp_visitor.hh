#ifndef IMP_VISITOR
#define IMP_VISITOR

#include "imp.hh"

class ImpVisitor {
public:
  virtual void visit(Program* e) = 0;
  virtual void visit(StatementList* e) = 0;
  virtual void visit(AssignStatement* e) = 0;
  virtual void visit(PrintStatement* e) = 0;
  
  virtual int visit(BinaryExp* e) = 0;
  virtual int visit(NumberExp* e) = 0;
  virtual int visit(IdExp* e) = 0;
  virtual int visit(ParenthExp* e) = 0;
};


#endif
