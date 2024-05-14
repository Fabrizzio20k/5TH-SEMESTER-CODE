#ifndef IMP_PARSER
#define IMP_PARSER

#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>

#include <unordered_map>

#include "imp.hh"
//#include "imp_ast.hh"

using namespace std;

class Token {
public:
  enum Type { LPAREN=0, RPAREN, PLUS, MINUS, MULT, DIV, EXP, NUM, ID, PRINT, SEMICOLON, ASSIGN, ERR, END };
  static const char* token_names[14]; 
  Type type;
  string lexema;
  Token(Type);
  Token(Type, const string source);
};


class Scanner {
public:
  Scanner(const char* in_s);
  Token* nextToken();
  ~Scanner();  
private:
  string input;
  int first, current;
  unordered_map<string, Token::Type> reserved;
  char nextChar();
  void rollBack();
  void startLexema();
  string getLexema();
  Token::Type checkReserved(string);
};

class Parser {
private:
  Scanner* scanner;
  Token *current, *previous;
  bool match(Token::Type ttype);
  bool check(Token::Type ttype);
  bool advance();
  bool isAtEnd();
  Program* parseProgram();
  Stm* parseStatement();
  Exp* parseExpression();
  Exp* parseTerm();
  Exp* parseFExp();
  Exp* parseFactor();

public:
  Parser(Scanner* scanner);
  Program* parse();

};




#endif
