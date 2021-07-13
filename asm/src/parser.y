%{
#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include <map>
#include <regex>
#include <tuple>
#include <limits>
#include <vector>
#include <utility>
#include <fstream>
#include <optional>
#include <iostream>
#include <filesystem>

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
	Ascii,

	Value8,
	Value16,

	Variable8,
	Variable16,
};

std::vector<std::uint8_t> assemble(const Instruction& insn) noexcept
{

}


std::uint16_t ident2int(const std::string& str, std::map<std::string, std::pair<Qualifier, std::uint16_t>>& idents) noexcept
{
	auto res = idents.at(str);

	switch (res.first)
	{
	case Qualifier::Macro:
		std::cout << "Macro identifiers cannot be converted implicitly to integer";
		std::exit(5);
		break;

	case Qualifier::Ascii:
		std::cout << "Ascii identifiers cannot be converted implicitly to integer";
		std::exit(5);
		break;

	case Qualifier::Value8:     [[fallthrough]];
	case Qualifier::Value16:    [[fallthrough]];
		return res.second;
		break;

	case Qualifier::Variable8:
		return std::uint16_t{ code[res.second] };
		break;

	case Qualifier::Variable16:
		auto lower = code[static_cast<int>(res.second) + 0];
		auto upper = code[static_cast<int>(res.second) + 1];
		return std::uint16_t{ (lower | (upper << 8)) };
	}
}

//maps any string identifier to a 16bit integer with a 'direct-ness' tag
std::map<std::string, std::pair<Qualifier, std::uint16_t>> identifiers{};

//stores all of the parsed macros
std::vector<Instruction> macros{};

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
%token T_PLUS T_MINUS T_TIMES T_DIVIDE T_LSHIFT T_RSHIFT T_AMPERSAND T_CARET T_TILDE T_PIPE
%token T_COMMA T_COLON T_LPAREN T_RPAREN T_LBRACE T_RBRACE T_LBRACK T_RBRACK T_EQUAL
%token T_HASH T_PERCENT
%token T_ORIGIN T_MACRO T_VAR8 T_VAR16 T_ALIAS8 T_ALIAS16 T_ASCII

%type<ival> imm
%type<ival> mem
%type<ival> number
%type<ival> operand
%type<ival> expression
%type<ival> paren_expr

%left T_PIPE
%left T_CARET
%left T_AMPERSAND
%left T_PLUS T_MINUS
%left T_TIMES T_DIVIDE
%left T_LSHIFT T_RSHIFT
%left UNARY

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

	struct Expression
	{
		int ival;
		char* sval;
	};
%}

%%

program: statements { std::puts("Parsing..."); };

declarations_helper: declarations_helper T_COMMA T_IDENTIFIER
|					 T_IDENTIFIER;

declarations: declarations_helper
|			  %empty;

arguments_helper: arguments_helper T_COMMA number
|				  number;

arguments: arguments_helper
|		   %empty;

directive: T_MACRO T_IDENTIFIER T_LPAREN declarations T_RPAREN T_LBRACE statements_recorded T_RBRACE {
																								//identifiers[$2] = std::make_pair(Qualifier::Macro, macro_iden);
																								identifiers[$2] = { Qualifier::Macro, macro_iden };
																								macros.emplace_back()
																								++macro_iden;
																							}
|		   T_IDENTIFIER T_LPAREN arguments T_RPAREN {
														std::cout << "invoked a macro" << std::endl;

														//auto result = std::find(identifiers.begin(), identifiers.end(), { Qualifier::Macro, $1 });
														auto result = identifiers.find($1);
														if (result->first == Qualifier::Macro)
														{
															if (result not_eq identifiers.end()) [[likely]]
															{
																std::cout << result->second << std::endl;
															}

															else [[unlikely]]
															{

															}
														}
														
														else
														{
															std::cerr << "Macro " << $1 << " was invoked but was not previously defined";
															std::exit(4);
														}
													}
|		   T_ORIGIN number {
								write_head = $2;
						   }
|		   T_ALIAS8 T_IDENTIFIER T_COLON expression {
														//identifiers[$2] = std::make_pair(Qualifier::Value8, $4);
														contains(identifiers, $2, { Qualifier::Value8, $4 });
														code[write_head] = $4;
														++write_head;
													}
|		   T_ALIAS16 T_IDENTIFIER T_EQUAL expression {
														//identifiers[$2] = std::make_pair(Qualifier::Value16, $4);
														contains(identifiers, $2, { Qualifier::Value16, $4 });
														code[write_head] = ($4 & 0x00FF);
														++write_head;
														code[write_head] = ($4 & 0xFF00);
														++write_head;
													}
|		   T_VAR16 T_IDENTIFIER T_EQUAL expression {
														//identifiers[$2] = std::make_pair(Qualifier::Variable16, $4);
														contains(identifiers, $2, { Qualifier::Variable16, write_head });
														code[write_head] = ($4 & 0x00FF);
														++write_head;
														code[write_head] = ($4 & 0xFF00);
														++write_head;
												   }
|		   T_VAR8 T_IDENTIFIER T_EQUAL expression {
													//identifiers[$2] = std::make_pair(Qualifier::Variable8, $4);
													contains(identifiers, $2, { Qualifier::Variable8, write_head });
													code[write_head] = $4;
													++write_head;
												 }
|		   T_ASCII T_IDENTIFIER T_STRING {
											identifiers[$2] = std::make_pair(Qualifier::Ascii, write_head);

											auto chars = $3;
											auto count = (std::strlen(chars) + 1);
											std::memcpy(&code[write_head], &chars, count)
											write_head += count;
										 };


number: T_INT        { $$ = $1;            }
|		T_IDENTIFIER { $$ = ident2int($1); }

paren_expr: T_LPAREN expression T_RPAREN { $$ = $2; }
|			T_LBRACK expression T_RBRACK { $$ = $2; };

expression:	number                            { $$ =  $1;       }
|			paren_expr                        { $$ =  $1;       }
|			expression T_PLUS expression      { $$ =  $1 +  $3; }
|			expression T_MINUS expression     { $$ =  $1 -  $3; }
|			expression T_TIMES expression     { $$ =  $1 *  $3; }
|			expression T_DIVIDE expression    { $$ =  $1 /  $3; }
|			expression T_LSHIFT expression    { $$ =  $1 << $3; }
|			expression T_RSHIFT expression    { $$ =  $1 >> $3; }
|			expression T_CARET expression     { $$ =  $1 ^  $3; }
|			expression T_AMPERSAND expression { $$ =  $1 &  $3; }
|			expression T_PIPE expression      { $$ =  $1 |  $3; }
|			T_MINUS expression %prec UNARY    { $$ = -$2;       }
|			T_TILDE expression %prec UNARY    { $$ = ~$2;       }

label: T_IDENTIFIER T_COLON;

operand: imm        { $$ = $1; }
|		 mem        { $$ = $1; }
|		 T_REGISTER { $$ = $1; }

instruction: T_IDENTIFIER
|			 T_IDENTIFIER operand
|			 T_IDENTIFIER operand T_COMMA operand;

statement: instruction
|		   directive
|		   label;

statement_with_endl: statement T_ENDL
|					 statement T_EOF
|					 T_ENDL;

statements: statements statement_with_endl
|			%empty;

statements_recorded: statements_recorded statement_with_endl
|					 %empty;

imm: T_HASH number     { $$ = $2; }
|	 T_HASH paren_expr { $$ = $2; };

mem: T_PERCENT number     { $$ = $2; }
|	 T_PERCENT paren_expr { $$ = $2; };

%%

int yyerror(const char *s)
{ 
	std::printf("%s\n", s);
	return 0;
}

int __cdecl main(const int argc, const char* const* const argv) noexcept
{
	//statement nterm
	//{ $$ = assemble($1); }
	//{ $$ = assemble($1); }
	//{ $$ = {};           }

	//statementes nterm
	//{ code[write_head] = $2; ++write_head; }

	//statements_recorded nterm
	//{ macros[macro_iden].emplace_back($2); }

	switch (argc)
	{
	case 2:
		//local string split function to use
		auto split = [&](const std::string& input, const std::string& delim) noexcept
		{
			std::size_t pos_start = 0, pos_end, delim_len = delim.length();
			std::string token;
			std::vector<std::string> res;

			while ((pos_end = input.find(delim, pos_start)) != std::string::npos)
			{
				token = input.substr(pos_start, pos_end - pos_start);
				pos_start = pos_end + delim_len;
				res.emplace_back(token);
			}

			res.emplace_back(input.substr(pos_start));
			return res;
		};

		//fetch the filepath off the command line
		std::string fp = argv[1];

		//open the file specified
		std::ifstream file(fp);

		//assert that the file supplied is readable
		if (!file.good())
		{
			std::cerr << "Failed to open the specified file";
			return 2;
		}

		//obtain the overall file size
		const auto length = std::filesystem::file_size(fp);

		//allocate and initialize a properly sized buffer
		std::string buffer(length, '\0');

		//read the entire file into the buffer
		file.read(buffer.data(), length);
		
		//ties together each basic component of a macro
		struct Macro { std::string identifier, statements; std::vector<std::string> arguments; };

		//local cache of macros that are found
		std::vector<Macro> macro_list{};

		//detects a valid macro declaration
		std::regex rx_macro_decl(R"(\.macro ([a-zA-Z_]\w*)\(([^\)]*)\)\s?\{([^\}]*)\})");

		//detects a valid macro invokation
		std::regex rx_macro_invk(R"(([a-zA-Z_]\w*)\([^\)]*\))");

		//will store each of the string regex matches that are made during the search
		std::smatch matches{};

		//discover all macro declarations present in the source file
		while (std::regex_search(buffer, matches, rx_macro_decl))
		{
			//remove any whitespace contained in the arguments list
			std::erase(std::remove_if(matches[2].begin(), matches[2].end(),
				[&](char c) { return std::isspace(static_cast<unsigned char>(c)); }), matches[2].end());

			//tokenize the string using the commas as delimiters
			macro_list.emplace_back({ matches[1], matches[3], split(matches[2], ",")});
		}

		//then erase the macro source code from the file
		std::regex_replace(buffer, rx_macro_decl, "");

		//discover all macro invokations present in the source file
		while (std::regex_search(buffer, matches, rx_macro_invk))
		{
			//get the raw string arguments list
			auto extracted = std::move(matches[2]);

			//remove any whitespace contained in the arguments list
			std::erase(std::remove_if(extracted.begin(), extracted.end(),
				[&](char c) { return std::isspace(static_cast<unsigned char>(c)); }), extracted.end());

			//tokenize the string using the commas as delimiters
			auto args = std::move(split(std::move(extracted), ","));

			//iterate over each of the arguments
			for (auto&& arg : args)
			{
				std::regex_replace(buffer, std::regex(), )
			}
		}

		//lex and parse the file contents
		do
		{
			yyparse();
		} while(!std::feof(yyin));
		break;

	default:
		//otherwise throw an error
		std::cerr << "Usage: assemble [filepath]";
		return 1;
	}

	//indicate program success
	return 0;
}