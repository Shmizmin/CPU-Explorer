%option noyywrap

%{
#include <iostream>
#include <string>
#include <cstring>

#include "parser.tab.hpp"

#define YY_DECL int yylex()

int line_number = 1;

enum
{
	R0A, R1A, R2A, R3A,
	R0L, R1L, R2L, R3L,
	R0H, R1H, R2H, R3H
};
%}

whitespace      [ \t]
endl            \n
carriagereturn  \r
decimal         -?[0-9]+
hexadecimal     -?"$"[0-9a-fA-F]+
identifier      [a-zA-Z][a-zA-Z_0-9]+
comment         \;[^\n]*
binary          -?"@"[0-1]+
stringliteral   ((\"|\')(([^\"]|\\\")*[^\\])?(\"|\'))

%%

{carriagereturn}	{ /*std::cout << "Flex found a carriage return\n";*/ }
{whitespace}		{ /*std::cout << "Flex found whitespace\n";*/ }

{comment}			{ /*std::cout << "Flex found a comment\n";*/ }
{endl}				{ line_number++; return T_ENDL; }

{decimal}			{
						/*std::cout << "Flex found a decimal int: " << yytext;*/
						yylval.ival = static_cast<int>(std::stoul(std::string(yytext), nullptr, 10));
						/*std::cout << " - " << yylval.ival << '\n';*/
						return T_INT;
					}

{identifier}		{
						/*std::cout << "Flex found an identifier: " << yytext << '\n';*/
						static auto tmp = std::string(yytext);
						yylval.sval = &tmp[0];
						return T_IDENTIFIER;
					}

{stringliteral}		{
						/*std::cout << "Flex found a string literal: " << yytext << '\n';*/
						static auto tmp = std::string(yytext).substr(1, std::strlen(yytext) - 2);
						yylval.sval = &tmp[0];
						return T_STRING;
					}

{hexadecimal}		{
						/*std::cout << "Flex found a hexadecimal int: " << yytext;*/
						yylval.ival = static_cast<int>(std::stoul(std::string(yytext).substr(1), nullptr, 16));
						/*std::cout << " - " << yylval.ival << '\n';*/
						return T_INT;
					}

{binary}			{
						/*std::cout << "Flex found a binary int: " << yytext;*/
						yylval.ival = static_cast<int>(std::stoul(std::string(yytext).substr(1), nullptr, 2));
						/*std::cout << " - " << yylval.ival << '\n';*/
						return T_INT;
					}

"%"  { return T_PERCENT; }
"#"  { return T_HASH;    }
"+"  { return T_PLUS;    }
"-"  { return T_MINUS;   }
"*"  { return T_TIMES;   }
"/"  { return T_DIVIDE;  }
"("  { return T_LPAREN;  }
")"  { return T_RPAREN;  }
"{"  { return T_LBRACE;  }
"}"  { return T_RBRACE;  }
"="  { return T_EQUAL;   }
":"  { return T_COLON;   }
","  { return T_COMMA;   }
"^"  { return T_CARET;   }
"<<" { return T_LSHIFT;  }
">>" { return T_RSHIFT;  }

".ascii" { return T_ASCII; }
".macro" { return T_MACRO; }
".org" { return T_ORIGIN; }

".alias8" { return T_ALIAS8; }
".alias16" { return T_ALIAS16; }
".var8" { return T_VAR8; }
".var16" { return T_VAR16; }

([rR]\0[hH])    { yylval.ival = (int)R0H; return T_REGISTER; }
([rR]\1[hH])    { yylval.ival = (int)R1H; return T_REGISTER; }
([rR]\2[hH])    { yylval.ival = (int)R2H; return T_REGISTER; }
([rR]\3[hH])    { yylval.ival = (int)R3H; return T_REGISTER; }

([rR]\0[lL])    { yylval.ival = (int)R0L; return T_REGISTER; }
([rR]\1[lL])    { yylval.ival = (int)R1L; return T_REGISTER; }
([rR]\2[lL])    { yylval.ival = (int)R2L; return T_REGISTER; }
([rR]\3[lL])    { yylval.ival = (int)R3L; return T_REGISTER; }

([rR]\0([aA])?) { yylval.ival = (int)R0A; return T_REGISTER; }
([rR]\1([aA])?) { yylval.ival = (int)R1A; return T_REGISTER; }
([rR]\2([aA])?) { yylval.ival = (int)R2A; return T_REGISTER; }
([rR]\3([aA])?) { yylval.ival = (int)R3A; return T_REGISTER; }

%%