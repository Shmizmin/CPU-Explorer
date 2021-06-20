%{
#include <cstdio>

#define YY_DECL extern "C" int yylex()

int line_number = 1;

enum
{
	R0A, R1A, R2A, R3A,
	R0L, R1L, R2L, R3L,
	R0H, R1H, R2H, R3H
};
%}

%option noyywrap

whitespace      [ \t]
endl            \n
hexadecimal     "#"?-?"$"[0-9a-fA-F]+
decimal         "#"?-?[0-9]+
binary          "#"?-?"@"[0-1]+
identifier      [a-zA-Z][a-zA-Z_0-9]+

%%

{whitespace} ;
{endl} { line_number++; return T_ENDL; }

{identifier}	{
					std::cout << "Flex found an identifier: " << yytext << '\n';
					yylval.sval = std::strdup(yytext);
					return T_IDENT;
				}

{decimal}	{
				std::cout << "Flex found a decimal int: " << yytext << '\n';
				yylval.ival = static_cast<int>(std::stoul(std::string(yytext), nullptr, 10));
				return T_INT;
			}

{hexadecimal}	{
					std::cout << "Flex found a hexadecimal int: " << yytext << '\n';
					yylval.ival = static_cast<int>(std::stoul(std::string(yytext).substr(1), nullptr, 10));
					return T_INT;
				}

{binary}	{
				std::cout << "Flex found a binary int: " << yytext << '\n';
				yylval.ival = static_cast<int>(std::stoul(std::string(yytext).substr(1), nullptr, 2));
				return T_INT;
			}

[ \t]+ { }
(";"[^\n\r]*) { }

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
"<<" { return T_LSHIFT;  }
">>" { return T_RSHIFT;  }

".ascii" { return T_ASCII; }
".macro" { return T_MACRO; }
".alias" { return T_ALIAS; }
".org" { return T_ORIGIN; }
".var" { return T_VAR; }

\n { return T_NEWLINE; }

([rR]"0"[hH])    { yylval = (int)R0H; return T_REGISTER; }
([rR]"1"[hH])    { yylval = (int)R1H; return T_REGISTER; }
([rR]"2"[hH])    { yylval = (int)R2H; return T_REGISTER; }
([rR]"3"[hH])    { yylval = (int)R3H; return T_REGISTER; }

([rR]"0"[lL])    { yylval = (int)R0L; return T_REGISTER; }
([rR]"1"[lL])    { yylval = (int)R1L; return T_REGISTER; }
([rR]"2"[lL])    { yylval = (int)R2L; return T_REGISTER; }
([rR]"3"[lL])    { yylval = (int)R3L; return T_REGISTER; }

([rR]"0"([aA])?) { yylval = (int)R0A; return T_REGISTER; }
([rR]"1"([aA])?) { yylval = (int)R1A; return T_REGISTER; }
([rR]"2"([aA])?) { yylval = (int)R2A; return T_REGISTER; }
([rR]"3"([aA])?) { yylval = (int)R3A; return T_REGISTER; }

%%