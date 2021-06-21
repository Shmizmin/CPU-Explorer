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
decimal         -?[0-9]+
hexadecimal     -?"$"[0-9a-fA-F]+
identifier      [a-zA-Z][a-zA-Z_0-9]+
comment         \;[^\n]*
binary          -?"@"[0-1]+
stringliteral   (\"|\')([^\\\"]|\\.)*(\"\')

%%

{whitespace}	{ std::cout << "Flex found whitespace" << '\n'; }
{comment}		{ std::cout << "Flex found a comment: " << yytext << '\n'; }
{endl}			{ line_number++; return T_ENDL; }

{decimal}		{
					std::cout << "Flex found a decimal int: " << yytext << '\n';
					yylval.ival = static_cast<int>(std::stoul(std::string(yytext), nullptr, 10));
					return T_INT;
				}

{identifier}	{
					std::cout << "Flex found an identifier: " << yytext << '\n';
					auto char_count = (std::strlen(yytext) + 1);
					auto* tmp = new char[char_count];
					std::memcpy(tmp, yytext, char_count);
					return T_IDENTIFIER;
				}

{stringliteral} {
					std::cout << "Flex found a string literal: " << yytext << '\n';
					auto char_count = (std::strlen(yytext) - 1);
					auto* tmp = new char[char_count];
					std::memcpy(tmp, (yytext + sizeof(char) * 1), char_count);
					return T_STRING;
				}

{hexadecimal}	{
					std::cout << "Flex found a hexadecimal int: " << yytext << '\n';
					yylval.ival = static_cast<int>(std::string(yytext).substr(1), nullptr, 10);
					return T_INT;
				}

{binary}		{
					std::cout << "Flex found a binary int: " << yytext << '\n';
					yylval.ival = static_cast<int>(std::stoul(std::string(yytext).substr(1), nullptr, 2));
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
".alias" { return T_ALIAS; }
".org" { return T_ORIGIN; }
".var" { return T_VAR; }

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