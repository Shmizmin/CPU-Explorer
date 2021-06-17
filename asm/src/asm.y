%skeleton "lalr1.cc"
%require "3.2"

%defines
%define api.namespace { asm }
%define api.parser.class { asm_parser }
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define parse.error verbose

%code requires
{
	namespace asm
	{
		class Compiler;
		class Lexer;
	}
} //code requires


%lex-param { script::Lexer &lexer }
%lex-param { script::Compiler &compiler }
%parse-param { script::Lexer &lexer }
%parse-param { script::Compiler &compiler }

%locations
%initial-action
{
	@$.begin.filename = @$.end.filename = &compiler.file;
};

%define parse.trace
%define parse.error verbose

%code top
{
	#include "compiler.hpp"
	#include "lexer.hpp"
	#include "parser.hpp"

	static asm::Parser::symbol_type yylex(asm::Lexer& scanner, asm::Compiler& compiler)
	{
		return scanner.get_next_token();
	}

	using namespace asm;
}



%token END 0
%token INT IDENTIFIER REGISTER
%token PLUS MINUS TIMES DIVIDE LSHIFT RSHIFT
%token COMMA COLON LPAREN RPAREN
%token HASH PERCENT

%left PLUS MINUS
%left TIMES DIVIDE
%left LSHIFT RSHIFT

%%

program: statements;

statements: statements statement
		  | %empty;

expression: INT
		  | mem
		  | imm
		  | paren_expr
		  | expression PLUS expression
		  | expression MINUS expression
		  | expression TIMES expression
		  | expression DIVIDE expression
		  | expression LSHIFT expression
		  | expression RSHIFT expression;

paren_expr: LPAREN expression RPAREN;

label: IDENTIFIER COLON;

operand: REGISTER
	   | imm
	   | mem;

instruction: IDENTIFIER
		   | IDENTIFIER operand
		   | IDENTIFIER operand COMMA operand;

statement: instruction
		 | label;

imm: HASH INT
   | HASH paren_expr;

mem: PERCENT INT
   | PERCENT paren_expr;

%%

void asm::Parser::error(const location_type& l, const std::string& m)
{
	compiler.error(l, m);
}

//int __cdecl main(void) noexcept
//{
//	yyin = stdin;
//
//	do
//	{
//		yyparse();
//	} while (!std::feof(yyin));
//
//	return 0;
//}