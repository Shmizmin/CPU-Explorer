%{
#include <cstdio>
#include <cstdlib>
#include <iostream>

extern int yylex();
extern int yyparse();
extern std::FILE* yyin;

extern int line_number;

int yyerror(const char* s);
%}

%union
{
	int ival;
	char* sval;
}

%token T_ENDL
%token T_EOF 0
%token<ival> T_INT
%token<ival> T_REGISTER
%token<sval> T_IDENTIFIER
%token<sval> T_STRING
%token T_PLUS T_MINUS T_TIMES T_DIVIDE T_LSHIFT T_RSHIFT
%token T_COMMA T_COLON T_LPAREN T_RPAREN T_LBRACE T_RBRACE T_EQUAL
%token T_HASH T_PERCENT
%token T_ALIAS T_ORIGIN T_MACRO T_VAR T_ASCII

%left T_PLUS T_MINUS
%left T_TIMES T_DIVIDE
%left T_LSHIFT T_RSHIFT
%right T_CARET

%start program

%%

program: statements { std::puts("Parsing program"); };

arguments_helper: arguments_helper T_COMMA number
|				  number;

arguments: arguments_helper
|		   %empty;

assignment: T_IDENTIFIER T_EQUAL expression;

directive: T_MACRO T_IDENTIFIER T_LPAREN arguments T_RPAREN T_LBRACE statements T_RBRACE
|		   T_ORIGIN number
|		   T_ALIAS assignment
|		   T_VAR assignment
|		   T_ASCII T_STRING;


number: T_INT
|		T_IDENTIFIER;

paren_expr: T_LPAREN expression T_RPAREN;

expression: imm
|			mem
|			number
|			paren_expr
|			expression T_PLUS expression
|			expression T_MINUS expression
|			expression T_TIMES expression
|			expression T_DIVIDE expression
|			expression T_LSHIFT expression
|			expression T_RSHIFT expression
|			expression T_CARET expression

label: T_IDENTIFIER T_COLON

operand: imm
|		 mem;
|		 T_REGISTER

instruction: T_IDENTIFIER { std::puts("Parsing a zero-arg instruction"); }
|			 T_IDENTIFIER operand { std::puts("Parsing a one-arg instruction"); }
|			 T_IDENTIFIER operand T_COMMA operand { std::puts("Parsing a two-arg instruction"); };

statement: instruction T_ENDL
|		   directive T_ENDL { std::puts("Parsing a directive"); }
|		   label T_ENDL { std::puts("Parsing a label"); }

statements: statements statement T_ENDL
|			%empty;

imm: T_HASH number;
|	 T_HASH paren_expr;

mem: T_PERCENT number;
|	 T_PERCENT paren_expr;

%%

int yyerror(const char *s)
{
	std::printf("%s\n", s);
	return 0;
}

int __cdecl main(void) noexcept
{
	yyin = stdin;

	do
	{
		yyparse();
	} while(!std::feof(yyin));

	return 0;
}