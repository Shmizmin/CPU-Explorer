%{
#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include <vector>
#include <optional>
#include <iostream>
#include <unordered_map>

extern int yylex();
extern int yyparse();
extern std::FILE* yyin;

extern int line_number;

std::uint16_t write_head{ 0 };
std::vector<std::uint8_t> code{ 0 };
std::unordered_map<std::string, std::uint16_t> identifiers{};

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
	enum class Addr : unsigned
	{
		Direct,
		Indirect,
	};

	enum class Mode : unsigned
	{
		None,
		Memory,
		Register,
		Immediate,
	};

	enum class Size : unsigned
	{
		None,
		WordAll,
		ByteLow,
		ByteHigh,
	};

	enum class Opcode : unsigned
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

	struct ModRM
	{
		Size size;
		Mode mode1, mode2;
		Addr addr1, addr2;

		constexpr auto from_byte(std::uint8_t byte) noexcept
		{
			size  =	static_cast<Size>((byte & 0b11000000) >> 6);
			mode1 = static_cast<Mode>((byte & 0b00110000) >> 4);
			mode2 = static_cast<Mode>((byte & 0b00001100) >> 2);
			addr1 = static_cast<Addr>((byte & 0b00000010) >> 1);
			addr2 = static_cast<Addr>((byte & 0b00000001) >> 0);
		}

		constexpr auto to_byte(void) noexcept
		{
			auto   byte  =  static_cast<std::uint8_t>(0);
			       byte |= (static_cast<std::uint8_t>(size ) << 6);
			       byte |= (static_cast<std::uint8_t>(mode1) << 4);
			       byte |= (static_cast<std::uint8_t>(mode2) << 2);
			       byte |= (static_cast<std::uint8_t>(addr1) << 1);
			       byte |= (static_cast<std::uint8_t>(addr2) << 0);
			return byte;
		}

		constexpr ModRM(std::uint8_t byte) noexcept
			{ from_byte(byte); }
	};

	struct Operand
	{
		ModRM modrm;

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
|		   T_ASCII T_IDENTIFIER T_STRING {
											identifiers[$2] = write_head;

											for (auto c : std::string($3))
												code.emplace_back(c);
										 };


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