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

%token END 0
%token NEWLINE
%token <unsigned long long> INT 
%token <unsigned long long> REGISTER
%token <std::string> IDENTIFIER
%token PLUS MINUS TIMES DIVIDE LSHIFT RSHIFT
%token COMMA COLON LPAREN RPAREN LBRACE RBRACE
%token HASH PERCENT
%token EQUAL
%token ALIAS ORIGIN MACRO VAR

%left PLUS MINUS
%left TIMES DIVIDE LSHIFT RSHIFT

%locations

%start program

%%

program: statements
	   ;

arguments_helper: arguments_helper COMMA number
|				  number;

arguments: arguments_helper
|		   %empty;

assignment: IDENTIFIER EQUAL expression;

directive: MACRO IDENTIFIER LPAREN arguments RPAREN LBRACE statements RBRACE
|		   ALIAS assignment
|		   VAR assignment
|		   ORIGIN number;

number: INT
|		IDENTIFIER;

paren_expr: LPAREN expression RPAREN;

expression: imm
|			mem
|			number
|			paren_expr
|			expression PLUS expression
|			expression MINUS expression
|			expression TIMES expression
|			expression DIVIDE expression
|			expression LSHIFT expression
|			expression RSHIFT expression;

label: IDENTIFIER COLON;

operand: REGISTER
|		 imm
|		 mem;

instruction: IDENTIFIER
|			 IDENTIFIER operand
|			 IDENTIFIER operand COMMA operand;

statement: instruction
|		   directive
|		   label;

statements: statements statement
|			%empty;

imm: HASH INT
|	 HASH paren_expr;

mem: PERCENT INT
|	 PERCENT paren_expr;

%%


void assembler::Parser::error(const location_type& l, const std::string& err_message)
{
	std::cerr << "Error: " << err_message << " at " << l << "\n";
}