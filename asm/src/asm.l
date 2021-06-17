%{
	#include "parser.hpp"
	#include "lexer.hpp"
	#include "compiler.hpp"
	#include <string>
	
	using token = asm::Parser::token;

	#define yyterminate() asm::Parser::make_END(loc);

	static asm::location loc;

	using namespace asm;
%}

%x sstring
%x scomment

%option noyywrap
%option nodefault
%option c++
%option yyclass="Lexer"

%{
	#define YY_USER_ACTION  loc.columns(static_cast<int>(yyleng));
%}

%%

%{
	loc.step();
%}

[\t ] { ; }
([\t ;[^\n]) { ; }
((r[0-2](l|h)?)|(flags|sp)) { return REGISTER; }

((\$[0-9a-f]+)|(@[0-1]+)|([0-9]+))
{ 
	switch (yytext[1])
	{
		case '$': yylval = std::stroul(std::string(yytext).substr(2), nullptr, 16); break;
		case '@': yylval = std::stroul(std::string(yytext).substr(2), nullptr, 2);  break;
		default:  yylval = std::stroul(std::string(yytext).substr(1), nullptr, 10); break;
	}
	
	return INT;
}

"%" { return PERCENT; }

"#" { return HASH;    }
"+" { return PLUS;    }
"-" { return MINUS;   }
"*" { return TIMES;   }
"/" { return DIVIDE;  }
"(" { return LPAREN;  }
")" { return RPAREN;  }
":" { return COLON;   }
"<<" { return LSHIFT; }
">>" { return RSHIFT; }

%%