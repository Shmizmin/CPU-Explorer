%{
#include <string>

#include "scanner.hpp"
#undef  YY_DECL
#define YY_DECL int assembler::Scanner::yylex(assembler::Parser::semantic_type* const lval, assembler::Parser::location_type* loc)

using token = assembler::Parser::token;

#define yyterminate() return token::END

#define YY_NO_UNISTD_H

#define YY_USER_ACTION loc->step(); loc->columns(yyleng);
%}

%option debug
%option nodefault
%option yyclass="assembler::Scanner"
%option noyywrap
%option c++

%%
%{
	yylval = lval;
%}

[a-z] { return token::LOWER; }
[A-Z] { return token::UPPER; }

[a-zA-Z]+ { yylval->build<std::string>(yytext); return token::WORD; }
\n { loc->lines(); return token::NEWLINE; }

. { return token::CHAR; }

%%