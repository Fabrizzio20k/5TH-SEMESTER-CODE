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

class Token
{
public:
  enum Type
  {
    LPAREN = 0,
    RPAREN,
    PLUS,
    MINUS,
    MULT,
    DIV,
    POW,
    NUM,
    SIN,
    COS,
    LOG,
    PI,
    ID,
    ERR,
    END
  };
  static const char *token_names[15];
  Type type;
  string lexema;
  Token(Type);
  Token(Type, char c);
  Token(Type, const string source);
};

const char *Token::token_names[15] = {"LPAREN", "RPAREN", "PLUS", "MINUS", "MULT", "DIV", "POW", "NUM", "SIN", "COS", "LOG", "PI", "ID", "ERR", "END"};

Token::Token(Type type) : type(type) { lexema = ""; }

Token::Token(Type type, char c) : type(type) { lexema = c; }

Token::Token(Type type, const string source) : type(type)
{
  lexema = source;
}

std::ostream &operator<<(std::ostream &outs, const Token &tok)
{
  if (tok.lexema.empty())
    return outs << Token::token_names[tok.type];
  else
    return outs << Token::token_names[tok.type] << "(" << tok.lexema << ")";
}

std::ostream &operator<<(std::ostream &outs, const Token *tok)
{
  return outs << *tok;
}

class Scanner
{
public:
  Scanner(const char *in_s);
  Token *nextToken();
  ~Scanner();

private:
  string input;
  int first, current;
  char nextChar();
  void rollBack();
  void startLexema();
  string getLexema();
};

Scanner::Scanner(const char *s) : input(s), first(0), current(0) {}

Token *Scanner::nextToken()
{
  Token *token;
  char c;
  // consume whitespaces
  c = nextChar();
  while (c == ' ')
    c = nextChar();
  if (c == '\0')
    return new Token(Token::END);
  startLexema();
  if (isdigit(c))
  {
    c = nextChar();
    while (isdigit(c))
      c = nextChar();
    rollBack();
    token = new Token(Token::NUM, getLexema());
  }
  else if (isalpha(c))
  {
    if (c == 's' && input[current] == 'i' && input[current + 1] == 'n')
    {
      current += 2;
      token = new Token(Token::SIN);
    }
    else if (c == 'c' && input[current] == 'o' && input[current + 1] == 's')
    {
      current += 2;
      token = new Token(Token::COS);
    }
    else if (c == 'l' && input[current] == 'o' && input[current + 1] == 'g')
    {
      current += 2;
      token = new Token(Token::LOG);
    }
    else if (c == 'p' && input[current] == 'i')
    {
      current++;
      token = new Token(Token::PI);
    }
    else
    {
      c = nextChar();
      while (isalpha(c))
        c = nextChar();
      rollBack();
      token = new Token(Token::ID, getLexema());
    }
  }
  else if (strchr("+-*/()", c))
  {
    switch (c)
    {
    case '(':
      token = new Token(Token::LPAREN);
      break;
    case ')':
      token = new Token(Token::RPAREN);
      break;
    case '+':
      token = new Token(Token::PLUS);
      break;
    case '-':
      token = new Token(Token::MINUS);
      break;
    case '*':
      if (input[current] == '*')
      {
        token = new Token(Token::POW);
        current++;
      }
      else
        token = new Token(Token::MULT);
      break;
    case '/':
      token = new Token(Token::DIV);
      break;
    default:
      cout << "No deberia llegar aca" << endl;
    }
  }
  else
  {
    token = new Token(Token::ERR, c);
  }
  return token;
}

Scanner::~Scanner() {}

char Scanner::nextChar()
{
  int c = input[current];
  if (c != '\0')
    current++;
  return c;
}

void Scanner::rollBack()
{ // retract
  if (input[current] != '\0')
    current--;
}

void Scanner::startLexema()
{
  first = current - 1;
  return;
}

string Scanner::getLexema()
{
  return input.substr(first, current - first);
}

// ---------------------------------------------------

int main(int argc, const char *argv[])
{

  if (argc != 2)
  {
    cout << "Incorrect number of arguments" << endl;
    exit(1);
  }

  Scanner scanner(argv[1]);
  Token *tk = scanner.nextToken();
  while (tk->type != Token::END)
  {
    cout << "next token " << tk << endl;
    delete tk;
    tk = scanner.nextToken();
  }
  cout << "last token " << tk << endl;
  delete tk;
}
