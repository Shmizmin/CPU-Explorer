%{
	#include <cstdio>
	#include <cstdlib>
	
	extern int yylex();
	extern int yyparse();
	extern std::FILE* yyin;
	
	void yyerror(const char* message);
%}

%token END 0
%token INT IMM MEM IDENTIFIER COMMENT REGISTER
%token PLUS MINUS TIMES DIVIDE
%token COMMA COLON

%left PLUS MINUS
%left TIMES DIVIDE

%start program

%%

program: statements
;

statements: statements
|           statement
|           %empty
;

expression: IMM
|           LPAREN expression RPAREN
|           expression PLUS expression
|           expression MINUS expression
|           expression TIMES expression
|           expression DIVIDE expression
;

label: IDENTIFIER COLON
|      IDENTIFIER COLON COMMENT
;

operand: REGISTER
|        IMM
|        MEM
;

instruction: IDENTIFIER
|            IDENTIFIER operand
|            IDENTIFIER operand COMMA operand
|            IDENTIFIER                       COMMENT
|            IDENTIFIER operand               COMMENT
|            IDENTIFIER operand COMMA operand COMMENT
;

statement: instruction
|          label
|          COMMENT
|          %empty
;

int main(void) noexcept
{
	yyin = stdin;

	do
	{
		yyparse();
	} while (!std::feof(yyin));

	return 0;
}

void yyerror(const char* s)
{
	std::fprintf(stderr, "Parse error: %s\n", s);
	std::exit(1);
}