#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <fstream>
#include <unordered_map>

using namespace std;

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
    INT,
    FLOAT,
    ID,
    SEMICOLON,
    COMMA,
    EQUAL,
    ERR,
    END
  };
  static const char *token_names[16];
  Type type;
  string lexema;
  Token(Type);
  Token(Type, char c);
  Token(Type, const string source);
};

const char *Token::token_names[16] = {"LPAREN", "RPAREN", "PLUS", "MINUS", "MULT", "DIV",
                                      "POW", "NUM", "INT", "FLOAT", "ID", "SEMICOLON", "COMMA", "EQUAL", "ERR", "END"};

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

  unordered_map<string, Token::Type> reserved;

  char nextChar();
  void rollBack();
  void startLexema();
  string getLexema();
  Token::Type checkReserved(string lexema);
};

Scanner::Scanner(const char *s) : input(s), first(0), current(0)
{
  reserved["int"] = Token::INT;
  reserved["float"] = Token::FLOAT;
}

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
    c = nextChar();
    while (isalpha(c) || isdigit(c))
      c = nextChar();
    rollBack();

    string lex = getLexema();

    Token::Type ttype = checkReserved(lex);

    if (ttype != Token::ERR)
      token = new Token(ttype);
    else
      token = new Token(Token::ID, getLexema());
  }
  else if (strchr("+-*/^(),;=", c))
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
      token = new Token(Token::PLUS, c);
      break;
    case '-':
      token = new Token(Token::MINUS, c);
      break;
    case '*':
      token = new Token(Token::MULT, c);
      break;
    case '/':
      token = new Token(Token::DIV, c);
      break;
    case '^':
      token = new Token(Token::POW, c);
      break;
    case ',':
      token = new Token(Token::COMMA, c);
      break;
    case ';':
      token = new Token(Token::SEMICOLON, c);
      break;
    case '=':
      token = new Token(Token::EQUAL, c);
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
  current++;
  return c;
}

void Scanner::rollBack()
{ // retract
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

Token::Type Scanner::checkReserved(string lexema)
{
  // unordered_map<std::string,Token::Type>::const_iterator it = reserved.find (lexema);
  auto it = reserved.find(lexema);

  if (it == reserved.end())
    return Token::ERR;
  else
    return it->second;
}

/* ******** Parser *********** */

class Parser
{
private:
  Scanner *scanner;
  Token *current, *previous;
  bool match(Token::Type ttype);
  bool check(Token::Type ttype);
  bool advance();
  bool isAtEnd();
  void parsePrograma();
  void parseVarDecl();
  void parseDecl();
  void parseType();
  void parseSents();
  void parseSent();
  void parseExpression();
  void parseTerm();
  void parseFactor();

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
  // if (isAtEnd()) return false;
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
      cout << "Parse error, unrecognised character: " << current->lexema << endl;
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
  // get first token
  current = scanner->nextToken();
  if (check(Token::ERR))
  {
    cout << "Error en scanner - caracter invalido" << endl;
    exit(0);
  }

  parsePrograma();

  if (!match(Token::END))
  {
    cout << "Error: Esperaba fin de input, encontro: " << current << endl;
    exit(0);
  }

  if (current)
    delete current;
  return;
}

void Parser::parsePrograma()
{
  parseVarDecl();
  if (check(Token::SEMICOLON))
    advance();
  else
  {
    cout << "Expecting semicolon" << endl;
    exit(0);
  }
  parseSents();
}

void Parser::parseVarDecl()
{
  parseDecl();
  while (check(Token::COMMA))
  {
    advance();
    parseDecl();
  }
}

void Parser::parseDecl()
{
  parseType();
  if (!check(Token::ID))
  {
    cout << "Expecting identifier" << endl;
    exit(0);
  }
  advance();
}

void Parser::parseType()
{
  if (check(Token::INT) || check(Token::FLOAT))
  {
    advance();
  }
  else
  {
    cout << "Expecting type" << endl;
    exit(0);
  }
  return;
}

void Parser::parseSents()
{
  parseSent();
  while (check(Token::COMMA))
  {
    advance();
    parseSent();
  }
}

void Parser::parseSent()
{
  if (!check(Token::ID))
  {
    cout << "Expecting identifier" << endl;
    exit(0);
  }
  advance();
  if (!check(Token::EQUAL))
  {
    cout << "Expecting equal sign" << endl;
    exit(0);
  }
  advance();
  parseExpression();
  return;
}

void Parser::parseExpression()
{
  parseTerm();
  while (check(Token::PLUS) || check(Token::MINUS))
  {
    advance();
    parseTerm();
  }
  return;
}

void Parser::parseTerm()
{
  parseFactor();
  while (check(Token::MULT) || check(Token::DIV))
  {
    advance();
    parseFactor();
  }
  return;
}

void Parser::parseFactor()
{
  if (match(Token::NUM))
  {
    // int v = stoi(previous->lexema);
    return;
  }
  if (match(Token::LPAREN))
  {
    parseExpression();

    if (!match(Token::RPAREN))
    {
      cout << "Expecting right parenthesis" << endl;
      exit(0);
    }
    return;
  }
  cout << "Couldn't find match for token: " << current->lexema << endl;
  exit(0);
  return;
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

  Parser parser(&scanner);
  parser.parse();

  cout << "Parsing completo" << endl;
}
