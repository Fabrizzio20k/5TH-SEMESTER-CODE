
#include "exp_parser.hh"



const char* Token::token_names[10] = { "LPAREN" , "RPAREN", "PLUS", "MINUS",
				       "MULT", "DIV","EXP",
				       "NUM", "ERR", "END" };

Token::Token(Type type):type(type) { lexema = ""; }

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
  } else if (strchr("+-*/()", c)) {
    switch(c) {
    case '(': token = new Token(Token::LPAREN); break;
    case ')': token = new Token(Token::RPAREN); break;
    case '+': token = new Token(Token::PLUS); break;
    case '-': token = new Token(Token::MINUS); break;
    case '*':
      c = nextChar();
      if (c == '*') token = new Token(Token::EXP);
      else { rollBack(); token = new Token(Token::MULT); }
      break;
    case '/': token = new Token(Token::DIV); break;
    default: cout << "No deberia llegar aca" << endl;
    }
  } else {
    token = new Token(Token::ERR, getLexema());
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


// match and consume next token
bool Parser::match(Token::Type ttype) {
  if (check(ttype)) {
    advance();
    return true;
  }
  return false;
}

bool Parser::check(Token::Type ttype) {
  if (isAtEnd()) return false;
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

Exp* Parser::parse() {
  current = scanner->nextToken();
  if (check(Token::ERR)) {
      cout << "Error en scanner - caracter invalido" << endl;
      exit(0);
  }
  Exp* e = parseExpression();
  if (current->type != Token::END) {
    cout << "Esperaba fin-de-input, se encontro " << current << endl;
    delete e;
    e = NULL;
  }

  if (current) delete current;
  return e;
}

Exp* Parser::parseExpression() {
  Exp *e, *rhs;
  e = parseTerm();
  while(match(Token::MINUS) || match(Token::PLUS)) {
    Token::Type op = previous->type;
    BinaryOp binop = (op==Token::MINUS)? MINUS : PLUS;
    rhs = parseTerm();
    e = new BinaryExp(e, rhs, binop);
  }
  return e;
}

Exp* Parser::parseTerm() {
  Exp *e, *rhs;
  e = parseFExp();
  while(match(Token::MULT) || match(Token::DIV)) {
    Token::Type op = previous->type;
    BinaryOp binop = (op==Token::MULT)?MULT : DIV;
    rhs = parseFExp();
    e = new BinaryExp(e, rhs, binop);
  }
  return e;
}

Exp* Parser::parseFExp() {
  Exp *lhs, *rhs;
  lhs = parseFactor();
  if (match(Token::EXP)) {
    return new BinaryExp(lhs, parseFExp(), EXP);
  }
  return lhs; 
}

Exp* Parser::parseFactor() {
  if (match(Token::NUM)) {
    //return stoi(previous->lexema);
    return new NumberExp(stoi(previous->lexema));
  }
  if (match(Token::LPAREN)) {
    Exp* e = parseExpression();
    if (!match(Token::RPAREN)) {
      cout << "Expecting right parenthesis" << endl;
      exit(0);
    }
    // return e; is fine too
    return new ParenthExp(e);
  }
  cout << "Couldn't find match for token: " << current << endl;
  exit(0);
}






