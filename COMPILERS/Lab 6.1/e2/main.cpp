#include <iostream>
#include "ast.h"
#include "parser.cpp"
#include "scanner.h"
int main()
{
    yyparse();
}