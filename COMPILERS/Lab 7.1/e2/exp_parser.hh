#ifndef AEXP_PARSER
#define AEXP_PARSER

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>

#include "exp.hh"

using namespace std;

class Token {
public:
  enum Type { LPAREN=0, RPAREN, PLUS, MINUS, MULT, DIV, EXP, NUM, ERR, END };
  static const char* token_names[10]; 
  Type type;
  string lexema;
  Token(Type);
  Token(Type, const string source);
};

std::ostream& operator << ( std::ostream& outs, const Token* tok );

class Scanner {
public:
  Scanner(const char* in_s);
  Token* nextToken();
  ~Scanner();  
private:
  string input;
  int first, current;
  char nextChar();
  void rollBack();
  void startLexema();
  string getLexema();
};

class Parser {
private:
  Scanner* scanner;
  Token *current, *previous;
  bool match(Token::Type ttype);
  bool check(Token::Type ttype);
  bool advance();
  bool isAtEnd();
  Exp* parseExpression();
  Exp* parseTerm();
  Exp* parseFExp();
  Exp* parseFactor();
public:
  Parser(Scanner* scanner);
  Exp* parse();
};








#endif
