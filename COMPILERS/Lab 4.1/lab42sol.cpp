#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>

using namespace std;

/*
  Scanner and Parser code inspired by code in Crafting Interpreters by Robert Nystrom
*/



class Token {
public:
  enum Type { LPAREN=0, RPAREN, PLUS, MINUS, MULT, DIV, POW, NUM, ERR, END };
  static const char* token_names[10]; 
  Type type;
  string lexema;
  Token(Type);
  Token(Type, char c);
  Token(Type, const string source);
};

const char* Token::token_names[10] = { "LPAREN" , "RPAREN", "PLUS", "MINUS", "MULT", "DIV",
				       "POW", "NUM", "ERR", "END" };

Token::Token(Type type):type(type) { lexema = ""; }

Token::Token(Type type, char c):type(type) { lexema = c; }

Token::Token(Type type, const string source):type(type) {
  lexema = source;
}

std::ostream& operator << ( std::ostream& outs, const Token & tok )
{
  if (tok.lexema.empty())
    return outs << Token::token_names[tok.type];
  else
    return outs << Token::token_names[tok.type] << "(" << tok.lexema << ")";
}

std::ostream& operator << ( std::ostream& outs, const Token* tok ) {
  return outs << *tok;
}


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

Scanner::Scanner(const char* s):input(s),first(0),current(0) { }

Token* Scanner::nextToken() {
  Token* token;
  char c;
  // consume whitespaces
  c = nextChar();
  while (c == ' ') c = nextChar();
  if (c == '\0') return new Token(Token::END);
  startLexema();
  if (isdigit(c)) {
    c = nextChar();
    while (isdigit(c)) c = nextChar();
    rollBack();
    token = new Token(Token::NUM, getLexema());
  } else if (strchr("+-*/^()", c)) {
    switch(c) {
    case '(': token = new Token(Token::LPAREN); break;
    case ')': token = new Token(Token::RPAREN); break;
    case '+': token = new Token(Token::PLUS,c); break;
    case '-': token = new Token(Token::MINUS,c); break;
    case '*': token = new Token(Token::MULT,c); break;
    case '/': token = new Token(Token::DIV,c); break;
    case '^': token = new Token(Token::POW,c); break;
    default: cout << "No deberia llegar aca" << endl;
    }
  } else {
    token = new Token(Token::ERR, c);
  }
  return token;
}

Scanner::~Scanner() { }

char Scanner::nextChar() {
  int c = input[current];
  current++;
  return c;
}

void Scanner::rollBack() { // retract
  current--;
}

void Scanner::startLexema() {
  first = current-1;
  return;
}

string Scanner::getLexema() {
  return input.substr(first,current-first);
}


/* ******** Parser *********** */

class Parser {
private:
  Scanner* scanner;
  Token *current, *previous;
  bool match(Token::Type ttype);
  bool check(Token::Type ttype);
  bool advance();
  bool isAtEnd();
  int  parseExpression();
  int  parseTerm();
  int parseFactor();
public:
  Parser(Scanner* scanner);
  int parse();
};


// match and consume next token
bool Parser::match(Token::Type ttype) {
  if (check(ttype)) {
    advance();
    return true;
  }
  return false;
}

bool Parser::check(Token::Type ttype) {
  //if (isAtEnd()) return false;
  return current->type == ttype;
}

bool Parser::advance() {
  if (!isAtEnd()) {
    Token* temp =current;
    if (previous) delete previous;
    current = scanner->nextToken();
    previous = temp;
    if (check(Token::ERR)) {
      cout << "Parse error, unrecognised character: " << current->lexema << endl;
      exit(0);
    }
    return true;
  }
  return false;
} 

bool Parser::isAtEnd() {
  return (current->type == Token::END);
} 

Parser::Parser(Scanner* sc):scanner(sc) {
  previous = current = NULL;
  return;
};

int Parser::parse() {
  // get first token
  current = scanner->nextToken();
  if (check(Token::ERR)) {
      cout << "Error en scanner - caracter invalido" << endl;
      exit(0);
  }
  int v = parseExpression();

  if (!match(Token::END)) {
    cout << "Error: Esperaba fin de input, encontro: " << current<< endl;
    exit(0);
  } 

  if (current) delete current;
  return v;
}

int Parser::parseExpression() {
  int accum, v;
  accum = parseTerm();
  while(match(Token::MINUS) || match(Token::PLUS)) {
    Token::Type op = previous->type;
    v = parseTerm();
    if (op == Token::PLUS) accum += v;
    else accum -= v;   
  }


  return accum;
}

int Parser::parseTerm() {
  int accum, v;
  accum = parseFactor();
  while(match(Token::MULT) || match(Token::DIV)) {
    Token::Type op = previous->type;
    v = parseFactor();
    if (op == Token::MULT) accum *= v;
    else accum /= v;
  }
  return accum;
}

int Parser::parseFactor() {
  if (match(Token::NUM)) {
    return stoi(previous->lexema);
  }
  if (match(Token::LPAREN)) {
    int v = parseExpression();
    if (!match(Token::RPAREN)) {
      cout << "Expecting right parenthesis" << endl;
      exit(0);
    }
    return v;
  }
  cout << "Couldn't find match for token: " << current->lexema << endl;
  exit(0);
  return 0;
}


// ---------------------------------------------------

int main(int argc, const char* argv[]) {
  
  if (argc != 2) {
    cout << "Incorrect number of arguments" << endl;
    exit(1);
  }

  Scanner scanner(argv[1]);

  Parser parser(&scanner);
  int v = parser.parse();

  cout << "CALC: " << v << endl;

  /*
  Token* tk = scanner.nextToken();
  while (tk->type != Token::END) {
    cout << "next token " << tk << endl;
    delete tk;
    tk =  scanner.nextToken();
  }
  cout << "last token " << tk << endl;
  delete tk;
  */
  
  
}


