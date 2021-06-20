%require  "3.2"

%{
#include <cstdio>
#include <cstdlib>
#include <iostream>

extern "C" int yylex();
extern "C" int yyparse();
extern "C" std::FILE* yyin;

extern int line_number;

void yyerror(const char *s);
%}

%union
{
	int ival;
	char* sval;
}

%token END 0
%token T_NEWLINE
%token<ival> T_INT
%token<ival> T_IMM
%token<ival> T_MEM
%token<ival> T_REGISTER
%token<sval> T_IDENTIFIER
%token T_PLUS T_MINUS T_TIMES T_DIVIDE T_LSHIFT T_RSHIFT
%token T_COMMA T_COLON T_LPAREN T_RPAREN T_LBRACE T_RBRACE T_EQUAL
%token T_ALIAS T_ORIGIN T_MACRO T_VAR T_ASCII

%left PLUS MINUS
%left TIMES DIVIDE
%left LSHIFT RSHIFT

%start program

%%

program: statements
	   ;

arguments_helper: arguments_helper T_COMMA number
|				  number;

arguments: arguments_helper
|		   %empty;

assignment: IDENTIFIER EQUAL expression;

directive: T_MACRO T_IDENTIFIER T_LPAREN arguments T_RPAREN T_LBRACE statements T_RBRACE
|		   T_ORIGIN number
|		   T_ALIAS assignment
|		   T_VAR assignment
|		   T_ASCII T_STRING;

number: T_INT
|		T_IDENTIFIER;

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

imm: T_HASH INT
|	 T_HASH paren_expr;

mem: T_PERCENT INT
|	 T_PERCENT paren_expr;

%%


void assembler::Parser::error(const location_type& l, const std::string& err_message)
{
	std::cerr << "Error: " << err_message << " at " << l << "\n";
}