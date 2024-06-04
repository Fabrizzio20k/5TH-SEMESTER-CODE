#include <iostream>

#include "svm.hh"

string snames[33] = { "push", "pop", "dup", "swap", "add", "sub", "mult", "div", "goto", "eq", "gt", "ge", "lt", "le", "skip", "store", "load", "print", "jmpz", "jmpn", "and", "or", "neg", "not", "halt", "call", "mark", "storer", "loadr", "pusha", "enter", "alloc", "return" };

Instruction::Instruction(string l, IType itype):label(l),type(itype),hasarg(false) {
}

Instruction::Instruction(string l, IType itype, int arg):label(l),type(itype),hasarg(true), argint(arg),jmplabel("") {
}

Instruction::Instruction(string l, IType itype, string arg):label(l),type(itype),hasarg(true),jmplabel(arg) {

}

SVM::SVM(list<Instruction*>& sl) {
  instructions.reserve(sl.size());
  copy(begin(sl), end(sl), back_inserter(instructions));
  sp = pc = 0;
  sp = max_stack = 0;
  hp = MEM_SIZE;
  ep = hp-1;
  // resolver labels
  for(int i=0; i < instructions.size(); i++) {
    string l = instructions[i]->label;
    if (l != "")
      labels[l] = i; 
  }
  for(int i=0; i < instructions.size(); i++) {
    string jl = instructions[i]->jmplabel;  // works for pusha too
    if (jl != "") {
      std::unordered_map<std::string,int>::const_iterator it = labels.find(jl);
      if (it == labels.end()) {
	cout << "No se encontro label " << jl << endl;
	exit(0);
      } else
	instructions[i]->argint = it->second;
    }
  }					   
}

void SVM::execute() {
  while (true) {   
    if (pc >= instructions.size()) {
      cout << "Program counter ouf bounds...exiting" << endl;
      break;
    }
    // cout << "pc " << pc << endl;
    // print_stack();
      if (!execute(instructions[pc])) break;
  }
}


bool SVM::execute(Instruction* instr) {
  Instruction::IType itype = instr->type;
  int next, top;
  //cout << "pc: " << pc << " fp " << fp << " sp " << sp << endl;
  //print_stack();
  if (itype==Instruction::IPOP || itype==Instruction::IDUP ||
      itype==Instruction::IPRINT || itype==Instruction::ISKIP) {
    switch (itype) {
    case(Instruction::IPOP):
      if (stack_empty()) svm_error("Can't pop from an empty stack");
      stack_pop(); break;
    case(Instruction::IDUP):
      if (stack_empty()) svm_error("Can't dup from an empty stack");
      stack_push(stack_top()); break;
    case(Instruction::IPRINT): cout << stack_top() << endl; stack_pop(); break;
    case(Instruction::ISKIP): break;
    default: svm_error("Programming Error 1");
    }
    pc++;
  } else if (itype==Instruction::IPUSH || itype==Instruction::ISTORE ||
	     itype==Instruction::ILOAD || itype==Instruction::IALLOC ||
	     itype==Instruction::ISTORER || itype==Instruction::ILOADR ||
	     itype==Instruction::IPUSHA)  {
    int arg = instr->argint;
    switch (itype) {
    case(Instruction::IPUSH): stack_push(instr->argint); break;
    case(Instruction::IPUSHA): stack_push(instr->argint); break;
    case (Instruction::ISTORE):
      if (stack_empty()) svm_error("Can't store from an empty stack");
      memory_write(instr->argint, stack_top()); stack_pop(); break;
      break;
    case (Instruction::ISTORER):
      if (stack_empty()) svm_error("Can't store from an empty stack");
      memory_write(fp+instr->argint, stack_top()); stack_pop(); break;
      break;  
    case(Instruction::ILOAD):
      stack_push(memory_read(instr->argint)); break;
      break;
    case(Instruction::ILOADR):
      stack_push(memory_read(fp+instr->argint)); break;
      break;
    case(Instruction::IALLOC):
      execute_alloc(arg);
      break;
    default: svm_error("Programming Error 2");
    }
    pc++;
  } else if (itype==Instruction::IJMPZ || itype==Instruction::IJMPN ) {
    bool jump = false;
    top = stack_top(); stack_pop();
    switch(itype) {
    case(Instruction::IJMPZ): jump = (top==0); break;
    case(Instruction::IJMPN): jump = (top!=0); break;
    default: svm_error("Programming Error 3");
    }
    if (jump) pc=instr->argint; else pc++;
  } else if
      (itype==Instruction::IADD || itype==Instruction::ISUB || itype==Instruction::IMUL ||
       itype==Instruction::IDIV || itype==Instruction::IEQ || itype==Instruction::IGT ||
       itype==Instruction::IGE  || itype==Instruction::ILT || itype==Instruction::ILE ||
       itype==Instruction::IAND  || itype==Instruction::IOR || itype==Instruction::ISWAP)  {
    top = stack_top(); stack_pop();
    next = stack_top(); stack_pop();    
    switch(itype) {
    case(Instruction::IADD): stack_push(next+top); break;
    case(Instruction::ISUB): stack_push(next-top); break;
    case(Instruction::IMUL): stack_push(next*top); break;
    case(Instruction::IDIV): stack_push(next/top); break;
    case(Instruction::IEQ): stack_push(next==top?1:0); break;
    case(Instruction::IGT): stack_push(next>top?1:0); break;
    case(Instruction::IGE): stack_push(next>=top?1:0); break;
    case(Instruction::ILT): stack_push(next<top?1:0); break;
    case(Instruction::ILE): stack_push(next<=top?1:0); break;
    case(Instruction::IAND): stack_push(next&&top?1:0); break;
    case(Instruction::IOR): stack_push(next||top?1:0); break;
    case(Instruction::ISWAP): stack_push(top); stack_push(next); break;
    default: svm_error("Programming Error 4");
    }
    pc++;
  } else if (itype == Instruction::IGOTO) {
    pc = instr->argint;
  } else if ((itype == Instruction::INEG) || (itype == Instruction::INOT)) {
    top = stack_top(); stack_pop();
    if (itype == Instruction::INEG) {
      top = -top;      
    } else {
      top = (top==0)?1:0;
    }
    stack_push(top);
    pc++;
  } else if (itype == Instruction::IMARK) {
    execute_mark();
    pc++;
  } else if (itype == Instruction::ICALL) {
    execute_call(); pc++;
  } else if (itype == Instruction::IENTER) {
    execute_enter(instr->argint); pc++;
  } else if (itype == Instruction::IRETURN) {
    execute_return(instr->argint); pc++;
  } else if (itype == Instruction::IHALT) {
    return false;
  }
  else {
    cout << "Programming Error: execute instruction" << endl;
    exit(0);
  }
  return true;
}

void SVM::print_stack() {
  int i = 0;
  cout << "stack [ ";
  while(i <= sp) {
    cout << stack[i++] << " ";  
  }
  cout << "]" << endl;  
}

void SVM::print() {
  for(int i= 0; i < instructions.size(); i++) {
    Instruction* s = instructions[i];
    if (s->label != "")
      cout << s->label << ": ";
    cout << snames[s->type] << " ";
    if (s->hasarg) {
      if (s->jmplabel == "")
	cout << s->argint;
      else {
	cout << s->jmplabel;
	// cout << "  [" << s->argint << "]";
      }
    }
    cout << endl;
  }					    
}

void SVM::stack_pop() {
  sp--;
}

void SVM::stack_push(int v) {
  sp++;
  stack[sp] = v;
}

int SVM::stack_top() {
  return stack[sp];
}

bool SVM::stack_empty() {
  return (sp == 0);
}

void SVM::memory_write(int addr,int v) {
  check_memory(addr);
  stack[addr] = v;
}
  
int SVM::memory_read(int addr) {
  check_memory(addr);
  return stack[addr];
}

    
void SVM::svm_error(string msg) {
  cout << "error: " << msg << endl;
  exit(0);
}

bool SVM::check_memory(int addr) {
  if (addr > sp) svm_error("Memory access outside allocated memory");
  if (addr <= 0) {
    cout << "Invalid: " << addr << endl;
    svm_error("Memory access: invalid address");
  }
  return true;
}

void SVM::execute_mark() {
  stack[++sp] = ep;
  stack[++sp] = fp;
  return;
}

void SVM::execute_call() {
  int tmp = pc;
  fp = sp;
  pc = stack[sp];
  stack[sp] = tmp;
  return;
}

void SVM::execute_enter(int m) {
  ep = sp +  m;
  if (ep >= hp) svm_error("Stack Overflow");
  return;
}

void SVM::execute_alloc(int n) {
  sp += n;
}

void SVM::execute_return(int n) {
  pc = stack[fp];
  ep = stack[fp-2];
  if (ep >= hp) svm_error("Stack Overflow");
  sp = fp-n;
  fp = stack[fp-1];
  return;
}




