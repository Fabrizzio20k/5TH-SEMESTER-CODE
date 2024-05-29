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

using namespace std;

class Token {
public:
  enum Type { LPAREN=0, RPAREN, PLUS, MINUS, MULT, DIV, EXP, LT, LTEQ, EQ, NUM, ID, PRINT, SEMICOLON, ASSIGN, IF, THEN, ELSE, ENDIF, ERR, END };
  static const char* token_names[21]; 
  Type type;
  string lexema;
  Token(Type);
  Token(Type, const string source);
};


class Scanner {
public:
  Scanner(string in_s);
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
  void parserError(string s);
  Program* parseProgram();
  StatementList* parseStatementList();
  Stm* parseStatement();
  Exp* parseExpression();
  Exp* parseCExp();
  Exp* parseAExp();
  Exp* parseTerm();
  Exp* parseFExp();
  Exp* parseFactor();

public:
  Parser(Scanner* scanner);
  Program* parse();

};




#endif
