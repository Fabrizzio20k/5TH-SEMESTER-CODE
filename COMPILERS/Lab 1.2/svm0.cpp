#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <fstream>

#include <list>
#include <stack>

using namespace std;

class Token
{
public:
  enum Type
  {
    LABEL,
    ID,
    KEYWORD,
    NUM,
    EOL,
    ERR,
    END
  };
  // agregar en KeywordType
  enum KeywordType
  {
    NOTHING,
    PUSH,
    POP,
    DUP,
    ADD,
    SUB,
    MUL,
    DIV,
    GOTO,
    JMP_LT
  };
  Type type;
  KeywordType ktype;
  string text;
  Token(Type);
  Token(Type, char c);
  Token(Type, const string &source, int first, int last);
};

class Scanner
{
private:
  string input;
  int first, current;

public:
  Scanner(const char *in_s);
  Scanner(const string s);
  Token *nextToken();
  ~Scanner();
};

Token::Token(Type type) : type(type), ktype(NOTHING) { text = ""; }

Token::Token(Type type, char c) : type(type), ktype(NOTHING) { text = c; }

Token::Token(Type type, const string &source, int first, int last) : type(type), ktype(NOTHING)
{
  text = source.substr(first, last);
}

std::ostream &operator<<(std::ostream &outs, const Token &tok)
{
  if (tok.text.empty())
    return outs << tok.type;
  else
    return outs << tok.type << "(" << tok.text << ")";
}

std::ostream &operator<<(std::ostream &outs, const Token *tok)
{
  return outs << *tok;
}

Scanner::Scanner(const char *s) : input(s), first(0), current(0) {}
Scanner::Scanner(const string s) : input(s), first(0), current(0) {}

Token *Scanner::nextToken()
{
  Token *token;
  // consume whitespaces
  while (input[current] == ' ')
    current++;
  if (input[current] == '\0')
    return new Token(Token::END);
  char c = input[current];
  first = current;
  if (isdigit(c))
  {
    current++;
    while (isdigit(input[current]))
      current++;
    token = new Token(Token::NUM, input, first, current - first);
  }
  else if (isalpha(c))
  {
    current++;
    c = input[current];
    while (isdigit(c) || isalpha(c) || c == '_')
    {
      current++;
      c = input[current];
    }
    if (input[current] == ':')
    {
      token = new Token(Token::LABEL, input, first, current - first);
      current++;
    }
    else
    {
      // check if it's an instruction
      string s = input.substr(first, current - first);
      Token::KeywordType ktype;
      // Token::KeywordType ktype = Scanner::checkReserved(text);
      // NOTHING, PUSH, POP, ADD, SUB, GOTO, JMP_LT };
      if (!s.compare("push"))
        ktype = Token::PUSH;
      else if (!s.compare("pop"))
        ktype = Token::POP;
      else if (!s.compare("dup"))
        ktype = Token::DUP;
      else if (!s.compare("add"))
        ktype = Token::ADD;
      else if (!s.compare("sub"))
        ktype = Token::SUB;
      else if (!s.compare("mul"))
        ktype = Token::MUL;
      else if (!s.compare("div"))
        ktype = Token::DIV;
      else if (!s.compare("goto"))
        ktype = Token::GOTO;
      else if (!s.compare("jmplt"))
        ktype = Token::JMP_LT;
      else
        ktype = Token::NOTHING;

      if (ktype != Token::NOTHING)
      {
        token = new Token(Token::KEYWORD, input, first, current - first);
        token->ktype = ktype;
      }
      else
        token = new Token(Token::ID, input, first, current - first);
    }
  }
  else if ((c == '\n') || (c == '\r'))
  {
    current++;
    c = input[current];
    while ((c == '\n') || (c == '\r'))
    {
      current++;
      c = input[current];
    }
    token = new Token(Token::EOL);
  }
  else
  {
    token = new Token(Token::ERR, c);
    current++;
  }
  return token;
}

Scanner::~Scanner() {}

// Instructions
class Instruction
{
public:
  // modificar InstrType
  enum InstrType
  {
    IPUSH,
    IPOP,
    IDUP,
    IADD,
    ISUB,
    IMUL,
    IDIV,
    IGOTO,
    IJMP_LT
  };
  string label;
  InstrType type;
  string gotoLabel;
  int argint; // could be label or int argument
  Instruction(string l, InstrType itype, int arg);
  static InstrType convertKeywordType(Token::KeywordType ktype);
};

Instruction::Instruction(string l, InstrType itype, int arg) : label(l), type(itype), argint(arg) {}

static Instruction::InstrType convertKeywordType(Token::KeywordType ktype)
{
  Instruction::InstrType itype;
  // { NOTHING, PUSH, POP, DUP, ADD, SUB, GOTO, JMP_LT };
  switch (ktype)
  {
  case (Token::PUSH):
    itype = Instruction::IPUSH;
    break;
  case (Token::POP):
    itype = Instruction::IPOP;
    break;
  case (Token::DUP):
    itype = Instruction::IDUP;
    break;
  case (Token::ADD):
    itype = Instruction::IADD;
    break;
  case (Token::SUB):
    itype = Instruction::ISUB;
    break;
  case (Token::MUL):
    itype = Instruction::IMUL;
    break;
  case (Token::DIV):
    itype = Instruction::IDIV;
    break;

  default:
    cout << "Error: Unknown Keyword type" << endl;
    exit(0);
  }
  return itype;
}

// Parser
class Parser
{
private:
  Scanner *scanner;
  Token *current, *previous;
  stack<int> stack;
  bool match(Token::Type ttype);
  bool check(Token::Type ttype);
  bool advance();
  bool isAtEnd();
  Instruction *parseInstruction();
  void evaluateInstruction(Instruction *instruction);

public:
  Parser(Scanner *scanner);
  void parse();
};

// match and consume next token
bool Parser::match(Token::Type ttype)
{
  if (check(ttype))
  {
    advance();
    return true;
  }
  return false;
}

bool Parser::check(Token::Type ttype)
{
  if (isAtEnd())
    return false;
  return current->type == ttype;
}

bool Parser::advance()
{
  if (!isAtEnd())
  {
    Token *temp = current;
    if (previous)
      delete previous;
    current = scanner->nextToken();
    previous = temp;
    if (check(Token::ERR))
    {
      cout << "Parse error, unrecognised character: " << current->text << endl;
      exit(0);
    }
    return true;
  }
  return false;
}

bool Parser::isAtEnd()
{
  return (current->type == Token::END);
}

Parser::Parser(Scanner *sc) : scanner(sc)
{
  previous = current = NULL;
  return;
};

void Parser::parse()
{
  current = scanner->nextToken();
  if (check(Token::ERR))
  {
    cout << "Error en scanner - caracter invalido" << endl;
    exit(0);
  }

  // parse and evaluate
  Instruction *instr = NULL;
  while (current->type != Token::END)
  {
    instr = parseInstruction();
    // execute instruction
    if (instr != NULL)
      evaluateInstruction(instr);
  }

  if (current)
    delete current;

  cout << "Stack: ";
  while (!stack.empty())
  {
    cout << stack.top() << " ";
    stack.pop();
  }
  cout << endl;
  return;
}

Instruction *Parser::parseInstruction()
{
  int arg = 0;
  string label;
  // Not matching labels yet
  if (match(Token::KEYWORD))
  {
    Token::KeywordType ktype = previous->ktype;
    // instrucciones sin argumentos
    // { NOTHING, PUSH, POP, DUP, ADD, SUB, GOTO, JMP_LT };
    // agregar en condicion
    if (ktype == Token::ADD || ktype == Token::SUB || ktype == Token::MUL || ktype == Token::DIV ||
        ktype == Token::DUP || ktype == Token::POP)
    {
    }
    else if (ktype == Token::PUSH)
    { // one arg
      if (match(Token::NUM))
      {
        arg = stoi(previous->text);
      }
      else
      {
        cout << "Error: expecting number after push instruction" << endl;
        exit(0);
      }
    }
    else
    {
      cout << "Instruction not implemented: " << previous->text << endl;
    }
    if (match(Token::EOL))
    {
      return new Instruction(label, convertKeywordType(ktype), arg);
    }
    else
    {
      cout << "Error: expecting end-of-line" << endl;
      exit(0);
    }
  }
  else
  {
    cout << "Parse error: Expecting instruction, found: " << current->text << endl;
    exit(0);
  }

  return NULL;
}

void Parser::evaluateInstruction(Instruction *instr)
{
  int x, y, v;
  Instruction::InstrType itype = instr->type;
  if (itype == Instruction::IPOP)
  {
    if (stack.empty())
    {
      cout << "Can't pop from an empty stack" << endl;
      exit(0);
    }
    stack.pop();
  }
  else if (itype == Instruction::IPUSH)
  {
    stack.push(instr->argint);
  }
  else if (itype == Instruction::IDUP)
  {
    x = stack.top();
    stack.push(x);
  }
  else
  {
    if (stack.size() < 2)
    {
      cout << "Can't perform binary operation" << endl;
      exit(0);
    }
    y = stack.top();
    stack.pop();
    x = stack.top();
    stack.pop();
    switch (itype)
    {
    case Instruction::IADD:
      v = x + y;
      break;
    case Instruction::ISUB:
      v = x - y;
      break;
    case Instruction::IMUL:
      v = x * y;
      break;
    case Instruction::IDIV:
      v = x / y;
      break;
    default:
      cout << "Instruction not implemented:" << itype << endl;
      exit(0);
    }
    stack.push(v);
  }
  return;
}

void test_scanner(Scanner *scanner)
{
  Token *tk = scanner->nextToken();
  while (tk->type != Token::END)
  {
    cout << "next token " << tk << endl;
    delete tk;
    tk = scanner->nextToken();
  }
  cout << "last token " << tk << endl;
  delete tk;
  exit(0);
}

int main(int argc, const char *argv[])
{

  if (argc != 2)
  {
    cout << "File name missing" << endl;
    exit(1);
  }
  cout << "Reading program from file " << argv[1] << endl;
  std::ifstream t(argv[1]);
  std::stringstream buffer;
  buffer << t.rdbuf();

  // our scanner takes strings
  Scanner scanner(buffer.str());

  // test_scanner(&scanner);

  Parser parser(&scanner);

  // En este caso, el parser realiza en analisis sintactico y
  // ejecuta cada instruccion a la vez
  parser.parse();
}
