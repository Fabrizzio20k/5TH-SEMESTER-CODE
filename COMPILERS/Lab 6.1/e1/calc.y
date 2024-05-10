%{
#include <iostream>
#include <string>
#include <cmath>
#include "Scanner.hpp"
%}

%require "3.7.4"
%language "C++"
%defines "Parser.hpp"
%output "Parser.cpp"

%define api.parser.class {Parser}
%define api.namespace {calc}
%define api.value.type variant
%param {yyscan_t scanner}

%code provides
{
    #define YY_DECL \
        int yylex(calc::Parser::semantic_type *yylval, yyscan_t yyscanner)
    YY_DECL;
}

%token EOL LPAREN RPAREN SIN COS TAN EXP LN POW SQRT
%token <int> INT


%right PLUS MINUS
%left MULT DIV
%left POW



%nterm <float>  iexp


%%

lines   : %empty
        | lines line
        ;

line    : EOL
        | iexp EOL         { std::cout << "Resultado:" <<  $1 << '\n'; }
        | error EOL        { yyerrok; }
        ;

iexp    : INT              { $$ = $1; }
        | iexp PLUS iexp   { $$ = $1+$3; }
        | iexp MINUS iexp  { $$ = $1-$3; }
        | iexp MULT iexp   { $$ = $1*$3; }
        | iexp DIV iexp    { $$ = $1/$3; }
        | iexp POW iexp    { $$ = pow($1, $3); }
        | SIN LPAREN iexp RPAREN         { $$ = sin($3 * M_PI/180); }
        | COS LPAREN iexp RPAREN         { $$ = cos($3); }
        | TAN LPAREN iexp RPAREN         { $$ = tan($3); }
        | EXP LPAREN iexp RPAREN         { $$ = exp($3); }
        | LN LPAREN iexp RPAREN          { $$ = log($3); }
        | SQRT LPAREN iexp RPAREN        { $$ = sqrt($3); }
        ;

%%

void calc::Parser::error(const std::string& msg) {
    std::cerr << msg << '\n';
}