%{
#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include <vector>
#include <utility>
#include <optional>
#include <iostream>
#include <map>

extern int yylex();
extern int yyparse();
extern std::FILE* yyin;

extern int line_number;

std::uint16_t macro_iden{ 1 },
			  write_head{ 0 };

std::vector<std::uint8_t> code{ 0 };

enum class Qualifier
{
	Macro,
	Address,

	Value8,
	Value16,

	Variable8,
	Variable16,
};

//maps any string identifier to a 16bit integer with a 'direct-ness' tag
std::map<std::string, std::pair<Qualifier, std::uint16_t>> identifiers{};

//stores any parsed macros in string form
std::vector<std::string> macros{};

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
%token T_ORIGIN T_MACRO T_VAR8 T_VAR16 T_ALIAS8 T_ALIAS16 T_ASCII

%left T_PLUS T_MINUS
%left T_TIMES T_DIVIDE
%left T_LSHIFT T_RSHIFT
%right T_CARET

%start program

%{
	enum class Mode : unsigned
	{
		None,
		Memory,
		Register,
		Immediate,
	};

	enum class Addr : unsigned
	{
		Direct,
		Indirect,
	};

	enum class Rsel : unsigned
	{
		None,
		R0,
		R1,
		R2
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
		Nop,

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
		Rsel rsel;
		Mode mode1, mode2;
		Addr addr1, addr2;

		constexpr auto from_byte(std::uint8_t byte) noexcept
		{
			rsel  = static_cast<Rsel>((byte & 0b11000000) >> 6);
			mode1 = static_cast<Mode>((byte & 0b00110000) >> 4);
			mode2 = static_cast<Mode>((byte & 0b00001100) >> 2);
			addr1 = static_cast<Addr>((byte & 0b00000010) >> 1);
			addr2 = static_cast<Addr>((byte & 0b00000001) >> 0);
		}

		constexpr auto to_byte(void) noexcept
		{
			auto   byte  =  static_cast<std::uint8_t>(0);
			       byte |= (static_cast<std::uint8_t>(rsel)  << 6);
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

	struct Mnemonic
	{
		Mode mode;
		Addr addr;
		Opcode opcode;
	};

	struct Instruction
	{
		Mnemonic mnemonic;
		std::optional<Operand> operand1, operand2;
	};

	auto contains(std::map<std::string, std::pair<Qualifier, std::uint16_t>>& map, 
		const std::string& key, std::pair<Qualifier, std::uint16_t> val) noexcept
	{
		if (map.contains(key)) [[unlikely]]
		{
			std::cerr << "Duplicate identifier " << key << " found\n";
			std::exit(3);
		}

		else [[likely]]
		{
			map.insert({ key, val });
		}
	}
%}

%%

program: statements { std::puts("Parsing program"); };

declarations_helper: declarations_helper T_COMMA T_IDENTIFIER
|					 T_IDENTIFIER;

declarations: declarations_helper
|			  %empty;

arguments_helper: arguments_helper T_COMMA number
|				  number;

arguments: arguments_helper
|		   %empty;

directive: T_MACRO T_IDENTIFIER T_LPAREN declarations T_RPAREN T_LBRACE statements T_RBRACE {
																								//identifiers[$2] = std::make_pair(Qualifier::Macro, macro_iden);
																								contains(identifiers, $2, { Qualifier::Macro, macro_iden });
																								macros.emplace_back($7);
																								++macro_iden;
																							}
|		   T_IDENTIFIER T_LPAREN arguments T_RPAREN {
														auto result = std::find(identifiers.begin(), identifiers.end(), { Qualifier::Macro, $1 });

														if (result not_eq identifiers.end()) [[likely]]
														{
															
														}

														else [[unlikely]]
														{

														}
													}
|		   T_ORIGIN number {
								write_head = $2;
						   }
|		   T_ALIAS8 T_IDENTIFIER T_COLON expression {
														//identifiers[$2] = std::make_pair(Qualifier::Value8, $4);
														contains(identifiers, $2, { Qualifier::Value8, $4 });
														code[write_head] = $4;
														write_head += 1;
													}
|		   T_ALIAS16 T_IDENTIFIER T_EQUAL expression {
														//identifiers[$2] = std::make_pair(Qualifier::Value16, $4);
														contains(identifiers, $2, { Qualifier::Value16, $4 });
														code[write_head] = ($4 & 0x00FF);
														write_head += 1;
														code[write_head] = ($4 & 0xFF00);
														write_head += 1;
													}
|		   T_VAR16 T_IDENTIFIER T_EQUAL expression {
														//identifiers[$2] = std::make_pair(Qualifier::Variable16, $4);
														contains(identifiers, $2, { Qualifier::Variable16, $4 });
														code[write_head] = ($4 & 0x00FF);
														write_head += 1;
														code[write_head] = ($4 & 0xFF00);
														write_head += 1;
												   }
|		   T_VAR8 T_IDENTIFIER T_EQUAL expression {
													//identifiers[$2] = std::make_pair(Qualifier::Variable8, $4);
													contains(identifiers, $2, { Qualifier::Variable8, $4 });
													code[write_head] = $4;
													write_head += 1;
												 }
|		   T_ASCII T_IDENTIFIER T_STRING {
											identifiers[$2] = std::make_pair(Qualifier::Address, write_head);

											auto chars = $3;
											auto count = (std::strlen(chars) + 1);
											std::memcpy(&code[write_head], &chars, count)
											write_head += count;
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