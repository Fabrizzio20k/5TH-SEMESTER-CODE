

#include "imp_parser.hh"


const char* Token::token_names[32] = {
  "LPAREN" , "RPAREN", "PLUS", "MINUS", "MULT","DIV","EXP","LT","LTEQ","EQ",
  "NUM", "ID", "PRINT", "SEMICOLON", "COMMA", "ASSIGN", "CONDEXP", "IF", "THEN", "ELSE", "ENDIF", "WHILE", "DO",
  "ENDWHILE", "ERR", "END", "VAR", "RETURN", "FUN", "ENDFUN", "TRUE", "FALSE" };

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


Scanner::Scanner(string s):input(s),first(0),current(0) {
  reserved["print"] = Token::PRINT;
  reserved["ifexp"] = Token::CONDEXP;
  reserved["if"] = Token::IF;
  reserved["then"] = Token::THEN;
  reserved["else"] = Token::ELSE;
  reserved["endif"] = Token::ENDIF;
  reserved["while"] = Token::WHILE;
  reserved["do"] = Token::DO;
  reserved["endwhile"] = Token::ENDWHILE;
  reserved["var"] = Token::VAR;
  reserved["return"] = Token::RETURN;
  reserved["fun"] = Token::FUN;
  reserved["endfun"] = Token::ENDFUN;
  reserved["true"] = Token::TRUE;
  reserved["false"] = Token::FALSE;
}

Token* Scanner::nextToken() {
  Token* token;
  char c;
  // consume whitespaces
  c = nextChar();
  while (c == ' ' || c == '\t'  || c == '\n') c = nextChar();
  if (c == '\0') return new Token(Token::END);
  startLexema();
  if (isdigit(c)) {
    c = nextChar();
    while (isdigit(c)) c = nextChar();
    rollBack();
    token = new Token(Token::NUM, getLexema());
  } else if (isalpha(c)) {
    c = nextChar();
    while (isalpha(c) || isdigit(c) || c=='_') c = nextChar();
    rollBack();
    string lex = getLexema();
    Token::Type ttype = checkReserved(lex);
    if (ttype != Token::ERR)
      token = new Token(ttype);
    else
      token = new Token(Token::ID, getLexema()); 
  } else if (strchr("()+-*/;=<,", c)) {
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
    case ';': token = new Token(Token::SEMICOLON); break;
    case ',': token = new Token(Token::COMMA); break;
    case '=': token = new Token(Token::ASSIGN); break;
      c = nextChar();
      if (c == '=') token = new Token(Token::EQ);
      else { rollBack(); token = new Token(Token::ASSIGN); }
      break;
    case '<':
      c = nextChar();
      if (c == '=') token = new Token(Token::LTEQ);
      else { rollBack(); token = new Token(Token::LT); }
      break;
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

Token::Type Scanner::checkReserved(string lexema) {
  std::unordered_map<std::string,Token::Type>::const_iterator it = reserved.find (lexema);
  if (it == reserved.end())
    return Token::ERR;
 else
   return it->second;
}

/* ***************** Parser ********************* */


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

void Parser::parserError(string s) {
  cout << "Parsing error: " << s << endl;
  exit(0);
}

Parser::Parser(Scanner* sc):scanner(sc) {
  previous = current = NULL;
  return;
};

Program* Parser::parse() {
  current = scanner->nextToken();
  if (check(Token::ERR)) {
      cout << "Error en scanner - caracter invalido" << endl;
      exit(0);
  }
  Program* p = parseProgram();
  if (current->type != Token::END) {
    cout << "Esperaba fin-de-input, se encontro " << current << endl;
    delete p;
    p = NULL;
    exit(0);
  }

  if (current) delete current;
  
  return p;
}

Program* Parser::parseProgram() {
  VarDecList* vdl = parseVarDecList();
  FunDecList* fdl = parseFunDecList();
  return new Program(vdl, fdl);
}

Body* Parser::parseBody() {
  VarDecList* vdl = parseVarDecList();
  StatementList* sl = parseStatementList();
  return new Body(vdl, sl);
}

VarDec* Parser::parseVarDec() {
  VarDec* vd = NULL;
  if (match(Token::VAR)) {
    if (!match(Token::ID)) parserError("Expecting type in var declaration");
    string var, type = previous->lexema;
    list<string> vars;
    if (!match(Token::ID)) parserError("Expecting id in var declaration");
    var = previous->lexema;
    vars.push_back(var);
    while(match(Token::COMMA)) {
      if (!match(Token::ID)) parserError("Expecting id in comma in var declaration");
      var = previous->lexema;
      vars.push_back(var);
    }
    if (!match(Token::SEMICOLON)) parserError("Expecting semicolon at end of var declaration");
    vd = new VarDec(type,vars);
  }
  return vd;
}

FunDec* Parser::parseFunDec() {
  FunDec* fd = NULL;
  if (match(Token::FUN)) {
    Body* body = NULL;
    if (!match(Token::ID)) parserError("Expecting return type in fun declaration");
    string rtype = previous->lexema;
    if (!match(Token::ID)) parserError("Expecting function name in fun declaration");
    string fname = previous->lexema;
    if (!match(Token::LPAREN)) parserError("Esperaba LPAREN en declaracion de funcion");
    list<string> types;
    list<string> vars;
    if (!check(Token::RPAREN)) {
      if (!match(Token::ID)) parserError("Expecting type in fun declaration");
      types.push_back(previous->lexema);
      if (!match(Token::ID)) parserError("Expecting identifier in fun declaration");
      vars.push_back(previous->lexema);
      while(match(Token::COMMA)) {
	types.push_back(previous->lexema);
	if (!match(Token::ID)) parserError("Expecting identifier in fun declaration");
	vars.push_back(previous->lexema);
      }     
    }
    if (!match(Token::RPAREN)) parserError("Esperaba RPAREN en declaracion de funcion");
    body = parseBody();
    if (!match(Token::ENDFUN)) parserError("Esperaba ENDFUN en declaracion de funcion");
    fd = new FunDec(fname, types, vars, rtype, body);
  }
  return fd;
}

VarDecList* Parser::parseVarDecList() {
  VarDecList* vdl = new VarDecList();
  VarDec* vd;
  vd = parseVarDec();
  while(vd != NULL) {
    vdl->add(vd);
    vd = parseVarDec();
  }
  return vdl;
}

FunDecList* Parser::parseFunDecList() {
  FunDecList* fdl = new FunDecList();
  FunDec* fd;
  fd = parseFunDec();
  while(fd != NULL) {
    fdl->add(fd);
    fd = parseFunDec();
  }
  return fdl;
}



StatementList* Parser::parseStatementList() {
  StatementList* p = new StatementList();
  p->add(parseStatement());
  while(match(Token::SEMICOLON)) {
    p->add(parseStatement());
  }
  return p;
}

/*
  id = exp
  print(x)
 */
Stm* Parser::parseStatement() {
  Stm* s = NULL;
  Exp* e = NULL;
  Body *tb, *fb;
  if (match(Token::ID)) {
    string lex = previous->lexema;
    if (!match(Token::ASSIGN)) {
      cout << "Error: esperaba =" << endl;
      exit(0);
    }
    s = new AssignStatement(lex, parseCExp());
    //memoria_update(lex, v);
  } else if (match(Token::PRINT)) {
    if (!match(Token::LPAREN)) {
      cout << "Error: esperaba ( " << endl;
      exit(0);
    }
    e = parseCExp();
    if (!match(Token::RPAREN)) {
      cout << "Error: esperaba )" << endl;
      exit(0);
    }
    s = new PrintStatement(e);
  } else if (match(Token::IF)) {
      e = parseCExp();
      if (!match(Token::THEN))
	parserError("Esperaba 'then'");
      tb = parseBody();
      fb = NULL;
      if (match(Token::ELSE)) {
	fb = parseBody();
      }
      if (!match(Token::ENDIF))
	parserError("Esperaba 'endif'");
      s = new IfStatement(e,tb,fb);
  } else if (match(Token::WHILE)) {
    e = parseCExp();
    if (!match(Token::DO))
      parserError("Esperaba 'do'");
    tb = parseBody();
    if (!match(Token::ENDWHILE))
	parserError("Esperaba 'endwhile'");
    s = new WhileStatement(e,tb);
  } else if (match(Token::RETURN)) {
    if (!match(Token::LPAREN)) parserError("Esperaba 'lparen'");
    if (!check(Token::RPAREN)) 
      e = parseCExp();
    if (!match(Token::RPAREN)) parserError("Esperaba 'rparen'");
    s = new ReturnStatement(e);
    
  } else {
    cout << "No se encontro Statement" << endl;
    exit(0);
  }
  return s;
}

Exp* Parser::parseCExp() {
  Exp *e, *rhs;
  e = parseExpression();
  if(match(Token::LT) || match(Token::LTEQ) ||
	match(Token::EQ)) {
    Token::Type op = previous->type;
    BinaryOp binop = (op==Token::LT)?LT:((op==Token::LTEQ)?LTEQ:EQ);
    rhs = parseExpression();
    e = new BinaryExp(e, rhs, binop);
  }
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
    return new NumberExp(stoi(previous->lexema));
  }
  if (match(Token::TRUE)) {
    return new TrueFalseExp(1);
  }
  if (match(Token::FALSE)) {
    return new TrueFalseExp(0);
  }
  if (match(Token::ID)) {
    string lex = previous->lexema;
    // function call
    if (match(Token::LPAREN)) {
      list<Exp*> args;
      if (!check(Token::RPAREN)) {
	args.push_back(parseCExp());
	while(match(Token::COMMA)) {
	  args.push_back(parseCExp());
	}
      }
      if (!match(Token::RPAREN)) parserError("Expecting rparen");
      return new FCallExp(lex,args);
    } else // id puro
      return new IdExp(lex);
  }
  if (match(Token::LPAREN)) {
    Exp* e = parseCExp();
    if (!match(Token::RPAREN))
      parserError("Expecting right parenthesis");
    return new ParenthExp(e);
  }
  if (match(Token::CONDEXP)) {
    if (!match(Token::LPAREN)) parserError("Expecting left parenthesis");
    Exp* c = parseCExp();
    if (!match(Token::COMMA)) parserError("Expecting comma");
    Exp* et = parseCExp();
    if (!match(Token::COMMA)) parserError("Expecting comma");
    Exp* ef = parseCExp();
    if (!match(Token::RPAREN)) parserError("Expecting right parenthesis");
    return new CondExp(c,et,ef);
  }
  cout << "Couldn't find match for token: " << current << endl;
  exit(0);
}









// ---------------------------------------------------



