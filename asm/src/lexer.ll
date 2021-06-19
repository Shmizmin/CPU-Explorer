%{
#include <string>

#include "scanner.hpp"
#undef  YY_DECL
#define YY_DECL int assembler::Scanner::yylex(assembler::Parser::semantic_type* const lval, assembler::Parser::location_type* loc)

using token = assembler::Parser::token;
using unsll = unsigned long long;

#define yyterminate() return token::END

#define YY_NO_UNISTD_H

#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

enum : unsll
{
	R0A, R1A, R2A, R3A,
	R0L, R1L, R2L, R3L,
	R0H, R1H, R2H, R3H
};
%}

%option debug
%option nodefault
%option yyclass="assembler::Scanner"
%option noyywrap
%option c++

integer { digit }+

%%

%{
	yylval = lval;
%}

[ \t]+ { }
(";".*) { }
[0-9]+ { yylval->build<unsll>(std::stoull(std::string(yytext), nullptr, 10)); return token::INT; }
("$"[0-9a-fA-F]+) { yylval->build<unsll>(std::stoull(std::string(&yytext[1], std::string::npos), nullptr, 16)); return token::INT; }
("@"[0-1]+) { yylval->build<unsll>(std::stoull(std::string(&yytext[1], std::string::npos), nullptr, 2)); return token::INT; }
 
"%"  { return token::PERCENT; }
"#"  { return token::HASH;    }
"+"  { return token::PLUS;    }
"-"  { return token::MINUS;   }
"*"  { return token::TIMES;   }
"/"  { return token::DIVIDE;  }
"("  { return token::LPAREN;  }
")"  { return token::RPAREN;  }
"{"  { return token::LBRACE;  }
"}"  { return token::RBRACE;  }
"="  { return token::EQUAL;   }
":"  { return token::COLON;   }
","  { return token::COMMA;   }
"<<" { return token::LSHIFT;  }
">>" { return token::RSHIFT;  }

".macro" { return token::MACRO; }
".alias" { return token::ALIAS; }
".org" { return token::ORIGIN; }
".var" { return token::VAR; }

\n { loc->lines(); return token::NEWLINE; }

([rR]"0"([hH])?) { yylval->build<unsll>(R0H); return token::REGISTER; }
([rR]"1"([hH])?) { yylval->build<unsll>(R1H); return token::REGISTER; }
([rR]"2"([hH])?) { yylval->build<unsll>(R2H); return token::REGISTER; }
([rR]"3"([hH])?) { yylval->build<unsll>(R3H); return token::REGISTER; }

([rR]"0"([lL])?) { yylval->build<unsll>(R0L); return token::REGISTER; }
([rR]"1"([lL])?) { yylval->build<unsll>(R1L); return token::REGISTER; }
([rR]"2"([lL])?) { yylval->build<unsll>(R2L); return token::REGISTER; }
([rR]"3"([lL])?) { yylval->build<unsll>(R3L); return token::REGISTER; }

([rR]"0"([aA])?) { yylval->build<unsll>(R0A); return token::REGISTER; }
([rR]"1"([aA])?) { yylval->build<unsll>(R1A); return token::REGISTER; }
([rR]"2"([aA])?) { yylval->build<unsll>(R2A); return token::REGISTER; }
([rR]"3"([aA])?) { yylval->build<unsll>(R3A); return token::REGISTER; }

[a-zA-Z][a-zA-Z_0-9]* { yylval->build<std::string>(yytext); return token::IDENTIFIER; }

%%