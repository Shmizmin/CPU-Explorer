%{
#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include <vector>
#include <optional>
#include <iostream>

extern int yylex();
extern int yyparse();
extern std::FILE* yyin;

extern int line_number;

extern std::vector<std::uint8_t> code;

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
%token T_COMMA T_COLON T_LPAREN T_RPAREN T_LBRACE T_RBRACE T_LBRACK T_RBRACK T_EQUAL
%token T_HASH T_PERCENT
//%token T_COMMENT
%token T_ALIAS T_ORIGIN T_MACRO T_VAR T_ASCII

%left T_PLUS T_MINUS
%left T_TIMES T_DIVIDE
%left T_LSHIFT T_RSHIFT
%right T_CARET

%start program

%{
	enum Addressing
	{
		Direct,
		Indirect,
	};

	enum Mode
	{
		None,
		Memory,
		Register,
		Immediate,
	};

	enum Size
	{
		None,
		WordAll,
		ByteLow,
		ByteHigh,
	};

	enum Opcode
	{
		Add,
		Subtract,
		Multiply,
		Divide,

		Xor,
		Or,
		And,
		Not,
		Compl,

		Load,
		Unload,
		Transfer,

		EnableInts,
		DisableInts,

		Interrupt,
		Reset,

		Push,
		Pop,
	};

	struct Operand
	{
		Size size;
		Mode mode1, mode2;
		Addressing addr1, addr2;

		union
		{
			std::uint8_t val8;
			std::uint16_t val16;
		};
	};

	struct Instruction
	{
		Opcode opcode;
		std::optional<Operand> operand1, operand2;
	};

%}

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
|		   T_ASCII T_STRING { for (auto c: std::string($2))  };


number: T_INT
|		T_IDENTIFIER;

paren_expr: T_LPAREN expression T_RPAREN { std::puts("Parsing a paren-expr"); }
|			T_LBRACK expression T_RBRACK { std::puts("Parsing a paren-expr"); };

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
|			expression T_CARET expression;

label: T_IDENTIFIER T_COLON;

operand: imm { std::puts("Parsing an operand"); }
|		 mem
|		 T_REGISTER;

instruction: T_IDENTIFIER { std::puts("Parsing a zero-arg instruction"); }
|			 T_IDENTIFIER operand { std::puts("Parsing a one-arg instruction"); }
|			 T_IDENTIFIER operand T_COMMA operand { std::puts("Parsing a two-arg instruction"); };

statement: instruction
|		   directive { std::puts("Parsing a directive"); }
|		   label { std::puts("Parsing a label"); };

statement_with_endl: statement T_ENDL
|					 statement T_EOF
|					 T_ENDL

statements: statements statement_with_endl
|			%empty;

imm: T_HASH number { std::puts("Parsing an immediate value"); }
|	 T_HASH paren_expr;

mem: T_PERCENT number { std::puts("Parsing a memory address"); }
|	 T_PERCENT paren_expr;

%%

int yyerror(const char *s)
{
	std::printf("%s\n", s);
	return 0;
}

int __cdecl main(int argc, const char** argv) noexcept
{
	//stores the assembled machine code
	std::vector<std::uint8_t> code{};

	//init the in file stream
	yyin = nullptr;

	//file handle to our stream
	std::FILE* handle = nullptr;

	//determine whether a filepath was supplied on the command line
	switch (argc)
	{
	case 2:
		//open the file specified
		handle = std::fopen(argv[1], "r");
		yyin = handle;
		break;

	case 1:
		//pipe the console stream in
		yyin = stdin;
		break;

	default:
		//otherwise throw an error
		std::cerr << "Usage: assemble [filepath]";
		return 1;
	}

	//verify that the file opening succedded
	if (yyin == nullptr)
	{
		std::cerr << "Failed to open the specified file";
		return 2;
	}

	//lex and parse the file contents
	do
	{
		yyparse();
	} while(!std::feof(yyin));

	//verify that the file handle is cleaned up properly
	if (handle != nullptr)
	{
		std::fclose(handle);
	}

	return 0;
}