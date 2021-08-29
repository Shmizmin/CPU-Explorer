%{
#include <cstdio>
#include <cstdint>
#include <cstdlib>

#include <map>
#include <set>
#include <regex>
#include <vector>
#include <utility>
#include <cassert>
#include <fstream>
#include <optional>
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <unordered_set>

extern int yylex();
extern int yyparse();
extern std::FILE* yyin;

extern int line_number;

std::uint16_t write_head{ 0 };

struct Instruction;

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

auto contains(std::map<std::string, std::pair<Qualifier, std::uint16_t>>& idents, std::string key, std::pair<Qualifier, std::uint16_t> value) noexcept
{
	if (idents.try_emplace(std::move(key), std::move(value)).first != idents.end()) [[likely]]
		;
	else
	{
		std::cerr << "Identifier " << key.c_str() << " was multiply defined";
		std::exit(50);
	}
}

std::uint16_t ident2int(const std::string& str, std::map<std::string, std::pair<Qualifier, std::uint16_t>>& idents) noexcept
{
	auto res = idents.at(str);

	switch (res.first)
	{
	case Qualifier::Macro:
		std::cout << "Macro identifiers cannot be converted implicitly to integer";
		std::exit(11);
		break;

	case Qualifier::Ascii:
		std::cout << "Ascii identifiers cannot be converted implicitly to integer";
		std::exit(12);
		break;

	case Qualifier::Value8:  [[fallthrough]];
	case Qualifier::Value16:
		return res.second;
		break;

	case Qualifier::Variable8:
		return std::uint16_t{ code[res.second] };
		break;

	case Qualifier::Variable16:
	{
		auto lower = code[static_cast<int>(res.second) + 0];
		auto upper = code[static_cast<int>(res.second) + 1];
		return (static_cast<std::uint16_t>(lower) | (static_cast<std::uint16_t>(upper) << 8));
	}

	default:
		std::cerr << "Unrecognized identifier with qualifier type ID of " << static_cast<int>(res.first) << " was encountered";
		std::exit(13);
	}
}

//maps any string identifier to a 16bit integer with a 'direct-ness' tag
std::map<std::string, std::pair<Qualifier, std::uint16_t>> identifiers{};

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
%token T_ORIGIN T_VAR8 T_VAR16 T_ALIAS8 T_ALIAS16 T_ASCII

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
%locations

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
%}

%%

program: statements { std::cout << "Parsing..."; }

directive: T_ORIGIN number {
								write_head = static_cast<std::uint16_t>($2);
						   }
|		   T_ALIAS8 T_IDENTIFIER T_COLON expression {
														contains(identifiers, $2, { Qualifier::Value8, static_cast<std::uint16_t>($4) });
														//code[write_head] = static_cast<std::uint8_t>($4);
														//++write_head;
													}
|		   T_ALIAS16 T_IDENTIFIER T_EQUAL expression {
														contains(identifiers, $2, { Qualifier::Value16, static_cast<std::uint16_t>($4) });
														//code[write_head] = static_cast<std::uint8_t>($4 & 0x00FF);
														//++write_head;
														//code[write_head] = static_cast<std::uint8_t>($4 & 0xFF00);
														//++write_head;
													}
|		   T_VAR16 T_IDENTIFIER T_EQUAL expression {
														contains(identifiers, $2, { Qualifier::Variable16, write_head });
														code[write_head] = static_cast<std::uint8_t>($4 & 0x00FF);
														++write_head;
														code[write_head] = static_cast<std::uint8_t>($4 & 0xFF00);
														++write_head;
												   }
|		   T_VAR8 T_IDENTIFIER T_EQUAL expression {
													contains(identifiers, $2, { Qualifier::Variable8, write_head });
													code[write_head] = static_cast<std::uint8_t>($4);
													++write_head;
												  }
|		   T_ASCII T_IDENTIFIER T_STRING {
											contains(identifiers, $2, { Qualifier::Ascii, write_head });
											auto chars = $3;
											auto count = (std::strlen(chars) + 1);
											std::memcpy(&code[write_head], &chars, count);
											write_head += static_cast<std::uint16_t>(count);
										 };


number: T_INT        { $$ = $1;                         }
|		T_IDENTIFIER { $$ = ident2int($1, identifiers); }

paren_expr: T_LPAREN expression T_RPAREN { $$ = $2; }
|			T_LBRACK expression T_RBRACK { $$ = $2; };

expression:	number                                        { $$ =  $1;       }
|			paren_expr                                    { $$ =  $1;       }
|			expression T_PLUS      expression             { $$ =  $1 +  $3; }
|			expression T_MINUS     expression             { $$ =  $1 -  $3; }
|			expression T_TIMES     expression             { $$ =  $1 *  $3; }
|			expression T_DIVIDE    expression             { $$ =  $1 /  $3; }
|			expression T_LSHIFT    expression             { $$ =  $1 << $3; }
|			expression T_RSHIFT    expression             { $$ =  $1 >> $3; }
|			expression T_CARET     expression             { $$ =  $1 ^  $3; }
|			expression T_AMPERSAND expression             { $$ =  $1 &  $3; }
|			expression T_PIPE      expression             { $$ =  $1 |  $3; }
|			T_MINUS                expression %prec UNARY { $$ = -$2;       }
|			T_TILDE                expression %prec UNARY { $$ = ~$2;       }

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

imm: T_HASH number     { $$ = $2; }
|	 T_HASH paren_expr { $$ = $2; };

mem: T_PERCENT number     { $$ = $2; }
|	 T_PERCENT paren_expr { $$ = $2; };

%%

int yyerror(const char* s)
{
	std::cout << s << '\n';
	return 0;
}

int __cdecl main(const int argc, const char** argv) noexcept
{
	switch (argc)
	{
	case 2:
	{
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

		//local string character escape function to use
		auto replace = [&](std::string& str, const std::string& from, const std::string& to) noexcept
		{
			if (from.empty()) return;
			std::size_t start_pos = 0;
			while((start_pos = str.find(from, start_pos)) != std::string::npos)
			{
				str.replace(start_pos, from.length(), to);
				start_pos += to.length();
			}
		};

		//fetch the filepath off the command line
		std::string fp(argv[1]);

		//open the file specified
		std::ifstream file(fp);

		//assert that the file supplied is readable
		if (!file.good())
		{
			std::cerr << "Failed to open the specified file for reading";
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

		//for freeing the macro code segment after preprocessing
		{
			//local cache of macros that are found
			std::map<std::string, Macro> macro_list{};

			//detects a valid macro declaration
			std::regex rx_macro_decl(R"(\.macro ([a-zA-Z_]\w*)\(([^\)]*)\)\s?\{([^\}]*)\})");

			//detects a valid macro invokation
			std::regex rx_macro_invk(R"(([a-zA-Z_]\w*)\(([^\)]*)\))");

			//detects a comment within a statement
			std::regex rx_stmt_cmmnt(R"((\s*([^\;]*)(\;(.*)?)?\s+))");

			//will store each of the string regex matches that are made during the search
			std::smatch matches{};

			//discover all macro declarations present in the source file
			while (std::regex_search(buffer, matches, rx_macro_decl))
			{
				//move the argument contents to a new string
				auto extracted = std::move(matches[2].str());

				//remove any comments that exist in the macro statements
				auto statements = std::regex_replace(matches[3].str(), rx_stmt_cmmnt, "$2\n");

				//remove any whitespace contained in the arguments list
				extracted.erase(std::remove_if(extracted.begin(), extracted.end(),
					[&](char c) { return std::isspace(static_cast<unsigned char>(c)); }), extracted.end());

				//tokenize the string using the commas as delimiters
				if (macro_list.try_emplace(std::move(matches[1].str()), Macro{ matches[1].str(), statements, split(extracted, ",") }).first != macro_list.end()) [[likely]]
				{
					//then erase the macro source code from the
					replace(buffer, matches[0].str(), "");
				}
				else
				{
					//if there was an errror, report it
					std::cerr << "Macro " << matches[1].str() << " was multiply defined";
					return 100;
				}
			}

			//discover all macro invokations present in the source file
			while (std::regex_search(buffer, matches, rx_macro_invk))
			{
				//move the argument contents to a new string
				auto extracted = std::move(matches[2].str());

				//remove any whitespace contained in the arguments list
				extracted.erase(std::remove_if(extracted.begin(), extracted.end(),
					[&](char c) { return std::isspace(static_cast<unsigned char>(c)); }), extracted.end());

				//tokenize the string using the commas as delimiters
				auto args = split(extracted, ",");

				//dynamically assert that the invoked macro actually exists
				{
					try
					{
						//verify that the correct number of arguments was supplied
						auto&& str = matches[1].str();
						auto m = macro_list.at(str);
						auto s = m.arguments.size();
						auto a = args.size();

						if (a != s) [[unlikely]]
						{
							std::cerr << "Macro " << str << " was supplied " << a << " arguments, but takes " << s << " arguments";
							return 110;
						}
					}

					//in the case that it doesn't
					catch (std::out_of_range&)
					{
						std::cerr << "Macro " << matches[1].str() << " was invoked, but not defined";
						return 120;
					}
				}

				//copy the whole invokation string for loop usage
 				auto whole = matches[0].str();

				//copy the identifier string for loop usage
				auto ident = matches[1].str();

				//iterate over each of the arguments
				for (auto i = 0; i < args.size(); ++i)
				{
					//perform the macro expansion using simple textual replacement
					replace(macro_list.at(ident).statements, macro_list.at(ident).arguments[i], args[i]);
				}

				//then commit the changes to the file buffer
				replace(buffer, whole, macro_list.at(ident).statements);
			}
		}

		//indicate preprocessed source filepath
		std::string path = (fp + ".pps");

		//saves the preprocessed source out to disk
		{
			//attempts to open up a file handle
			auto* fptr = std::fopen(path.c_str(), "w");

			//errors if not opened correctly
			if (fptr == NULL)
			{
				std::cerr << "Failed to open the specified file for writing";
				return 6;
			}

			//write out to disk and close the file handle
			std::fwrite(buffer.data(), sizeof(buffer[0]), buffer.size(), fptr);
			std::fclose(fptr);
		}

		//reads out the saved preprocessed source
		auto* fptr = std::fopen(path.c_str(), "r");
		auto  size = std::filesystem::file_size(path);
		std::string text(size, '\0');
		static_cast<void>(std::fread(text.data(), sizeof(text[0]), size, fptr));

		//detects a valid label declaration
		std::regex rx_label_decl(R"(^([a-zA-Z_][a-zA-Z0-9_]*)\:\s*(?:\;.*)?$)");

		//will store each of the string regex matches that are made during the search
		std::smatch matches{};

		//will store every label identifier discovered
		//std::map<std::string, int> labels{};
		std::unordered_set<std::string> labels{};

		//discover all label identifiers present in the source file
		auto reg = rx_label_decl;
		auto iter = std::sregex_iterator(text.begin(), text.end(), reg);
		std::sregex_iterator end;

		while (iter != end)
		{
			std::smatch match = *iter;

			if (!labels.emplace(match[1].str()).second) [[unlikely]]
			{
				std::cerr << "Duplicate label identifier " << matches[1].str() << " was found";
				return 7;
			}

			++iter;
		}

		//lex and parse the file contents
		do
		{
			yyparse();
		} while(!std::feof(yyin));
		break;
	}

	default:
		//otherwise throw an error
		std::cerr << "Usage: assemble [filepath]";
		return 1;
	}

	//indicate program success
	return 0;
}