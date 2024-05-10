%{
  #include <iostream>
  #include <cctype>
  #include <cstring>
  #include <vector>
  #include <stack>
  #include "ast.h"

  // Bring the standard library into the
  // global namespace
  using namespace std;

  // Prototypes to keep the compiler happy
  void yyerror (const char *error);
  int  yylex ();
  void clear_stack ();

  // stack class that takes care of all the nodes that were allocated
  stack <Expression *> nodes;
%}

%token NUMBER IDENT SIN COS TAN LOG EXP
%union {
  Expression *exp;  /* For the expressions. Since it is a pointer, no problem. */
  float       value;  /* For the lexical analyser. NUMBER tokens */
  char      ident;  /* For the lexical analyser. IDENT tokens */
}

/* Lets inform Bison about the type of each terminal and non-terminal */
%type <exp>   exp
%type <value> NUMBER

/* Precedence information to resolve ambiguity */
%left '+' '-'
%left '*' '/'
%%

prompt : exp  '\n'             {
                                 if ($1) {
                                   cout << $1->value () << endl;
                                   clear_stack ();
                                 }
                               }
       |  prompt  exp  '\n'    {
                                 if ($2) {
                                   cout << $2->value () << endl;
                                   clear_stack ();
                                 }
                               }
       | error '\n'            { clear_stack (); }
       ;

exp : exp '+' exp              {
                                 $$ = new Plus ($1, $3);
                                 nodes.pop ();  //  The childreen are handled by Plus so we
                                 nodes.pop ();  // take them of the allocated nodes stack.
                                 nodes.push ($$);
                               }
    | exp '-' exp              {
                                 $$ = new Minus ($1, $3);
                                 nodes.pop ();  //  The childreen are handled by Plus so we
                                 nodes.pop ();  // take them of the allocated nodes stack.
                                 nodes.push ($$);
                               }
    | exp '*' exp              {
                                 $$ = new Multiply ($1, $3);
                                 nodes.pop ();  //  The childreen are handled by Plus so we
                                 nodes.pop ();  // take them of the allocated nodes stack.
                                 nodes.push ($$);
                               }
    | exp '/' exp              {
                                 $$ = new Divide ($1, $3);
                                 nodes.pop ();  //  The childreen are handled by Plus so we
                                 nodes.pop ();  // take them of the allocated nodes stack.
                                 nodes.push ($$);
                               }
    | SIN '(' exp ')'          {
                                 $$ = new Trigonometric ($3, 's');
                                 nodes.pop ();  //  The childreen are handled by Plus so we
                                 nodes.push ($$);
                               }
    | COS '(' exp ')'          {
                                 $$ = new Trigonometric ($3, 'c');
                                 nodes.pop ();  //  The childreen are handled by Plus so we
                                 nodes.push ($$);
                               }
    | TAN '(' exp ')'          {
                                 $$ = new Trigonometric ($3, 't');
                                 nodes.pop ();  //  The childreen are handled by Plus so we
                                 nodes.push ($$);
                               }
    | LOG '(' exp ')'          {
                                 $$ = new Logarithm ($3, 'l');
                                 nodes.pop ();  //  The childreen are handled by Plus so we
                                 nodes.push ($$);
                               }
    | EXP '(' exp ')'          {
                                 $$ = new Logarithm ($3, 'e');
                                 nodes.pop ();  //  The childreen are handled by Plus so we
                                 nodes.push ($$);
                               }
    | NUMBER                   { 
                                $$ = new Number ($1); 
                                nodes.push ($$); 
                                }
    ;
%%

// we need to provid the following functions



void yyerror (const char *error)
{
  cout << error << endl;
}


// Deletes all the nodes that were allocated
void clear_stack ()
{
  while (!nodes.empty ()) {
    delete nodes.top ();
    nodes.pop ();
  }
}
