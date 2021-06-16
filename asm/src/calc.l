%option nyywrap

%{
#define YY_DECL int yylex()
#include "calc.tab.hpp"
#include <string>

void do_make(void) noexcept
{
	switch (yytext[1])
	{
		case '$': yylval = std::stoul(std::string(yytext).substr(2), nullptr, 16); break;
		case '@': yylval = std::stoul(std::string(yytext).substr(2), nullptr, 2);  break;
		default:  yylval = std::stoul(std::string(yytext).substr(1), nullptr, 10); break;
	}
}
%}

//register_helper: 'r'[0 - 2]
//
//register : register_helper
//| register_helper 'l'
//| register_helper 'h'
//| "sp"
//| "flags"
//;

%%

[\t ] ; //any whitespace is ignored
(#((\$[0-9a-f]+)|(@[0-1]+)|([0-9]+))) { do_make(); return IMM; }
(%((\$[0-9a-f]+)|(@[0-1]+)|([0-9]+))) { do_make(); return MEM; }
((r[0-2](l|h)?)|(flags|sp)) { return REGISTER; }

((\$[0-9a-f]+)|(@[0-1]+)|([0-9]+)) { do_make(); return INT; }

[0-9]+   { yylval = atoi(yytext); return INT;      }
\n       {                            return NEWLINE;  }
"+"      {                            return PLUS;     }
"-"      {                            return MINUS;    }
"*"      {                            return TIMES;    }
"/"      {                            return DIVIDE;   }
"("      {                            return LPAREN;   }
")"      {                            return RPAREN;   }
":"      {                            return COLON;    }

%%