%skeleton "lalr1.cc"
%require  "3.2"
%debug
%defines
%define api.namespace {assembler}
%define api.parser.class {Parser}

%code requires
{
	namespace assembler
	{
		class Driver;
		class Scanner;
	}

#ifndef YY_NULLPTR
#define YY_NULLPTR nullptr
#endif
}

%parse-param { Scanner &scanner }
%parse-param { Driver  &driver  }

%code
{
	#include <iostream>
	#include <cstdlib>
	#include <fstream>
	
	#include "driver.hpp"

	#undef yylex
	#define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert

%token               END    0     "end of file"
%token               UPPER
%token               LOWER
%token <std::string> WORD
%token               NEWLINE
%token               CHAR

%locations

%%

list_option: END
		   | list END
		   ;

list: item
	| list item
	;

item: UPPER   { driver.add_upper();    }
	| LOWER   { driver.add_lower();    }
	| WORD    { driver.add_word( $1 ); }
	| NEWLINE { driver.add_newline();  }
	| CHAR    { driver.add_char();     }
	;

%%


void assembler::Parser::error(const location_type& l, const std::string& err_message)
{
	std::cerr << "Error: " << err_message << " at " << l << "\n";
}