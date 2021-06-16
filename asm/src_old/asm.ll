%option noyywrap
%option c++
%option yyclass="Assembler"

%{
#define YYSTYPE double
#include "calc.tab.hh"
}%

white [ \t]+
digit [0-9]
integer { digit }+
exponent [eE][+-]?{ integer }
real { integer }("."{ integer })?{ exponent }

%%

{ white } { }
{ real } { yylval = std::atof(yytext); return NUMBER; }

"+" return PLUS;
"-" return MINUS;
"*" return TIMES;
"/" return DIVIDE;
"^" return POWER;
"(" return LPAREN;
")" return RPAREN;
"\n" return ENDL;