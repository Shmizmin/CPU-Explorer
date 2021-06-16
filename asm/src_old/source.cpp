#include <cstdlib>
#include <iostream>
#include "peglib.hpp"

int __cdecl main(int argc, const char** argv)
{
	if (argc < 2 || std::string("--help") == argv[1])
	{
		std::cerr << "Usage: calculate [expression]" << std::endl;
		return 1;
	}

	std::function<long(const peg::Ast&)> eval = [&](const peg::Ast& ast)
	{
		if (ast.name == "NUMBER")
		{
			return ast.token_to_number<long>();
		}

		else
		{
			const auto& nodes = ast.nodes;
			auto result = eval(*nodes[0]);

			for (auto i = 1u; i < nodes.size(); i += 2)
			{
				auto num = eval(*nodes[i + std::size_t{ 1 }]);
				auto ope = nodes[i]->token[0];

				switch (ope)
				{
					case '+': result += num; break;
					case '-': result -= num; break;
					case '*': result *= num; break;
					case '/': result /= num; break;
				}
			}
			return result;
		}
	};

	peg::parser parser
	(R"(
		PROGRAM       <-  STATEMENT*

		NAME          <-  [a-zA-Z_][a-zA-Z_0-9]*
		
		MNEMONIC      <-  ~%whitespace < NAME > ~%whitespace
		COMMENT       <-  ~%whitespace ';' [^\t\r\n\:]*
		
		LABEL         <-  ~%whitespace < NAME > ':'^nocolon ~COMMENT?
		
		DEC_NUMBER    <-  < [0-9]+ >
		HEX_NUMBER    <-  '$' < [0-9a-f]+ >
		BIN_NUMBER    <-  '@' < [0-1]+ >
		
		HEX_IMMEDIATE <-  '#' < HEX_NUMBER >
		DEC_IMMEDIATE <-  '#' < DEC_NUMBER >
		BIN_IMMEDIATE <-  '#' < BIN_NUMBER >
		
		HEX_MEMORY    <-  '%' < HEX_NUMBER >
		DEC_MEMORY    <-  '%' < DEC_NUMBER >
		BIN_MEMORY    <-  '%' < BIN_NUMBER >
		
		REGISTER      <-  < ('r'i[0-2]('l' / 'h')?) / ('sp'i) / ('flags'i) / ('all') >
		
		#NUMBER        <-  (DEC_NUMBER / HEX_NUMBER / BIN_NUMBER)
		IMMEDIATE     <-  (HEX_IMMEDIATE / DEC_IMMEDIATE / BIN_IMMEDIATE)
		MEMORY        <-  (HEX_MEMORY / DEC_MEMORY / BIN_MEMORY)
		
		OPERAND       <-  ~%whitespace (REGISTER / MEMORY / IMMEDIATE / NAME) ~%whitespace
		
		
		DIRECTIVE     <-  ~%whitespace '.' < NAME > ~COMMENT?
		
		INSTRUCTION   <-   (MNEMONIC OPERAND ',' OPERAND ~COMMENT?)
		                 / (MNEMONIC OPERAND             ~COMMENT?)
		                 / (MNEMONIC                     ~COMMENT?)
		
		STATEMENT     <-  (INSTRUCTION / LABEL / DIRECTIVE / ~COMMENT)
		
		
		nocolon       <-  '' { message "Labels must be followed by a colon" }
		
		%whitespace   <-  [ \t\r\n]*
	)");

	parser.enable_ast();

	auto expr = argv[1];
	std::shared_ptr<peg::Ast> ast;

	if (parser.parse(expr, ast))
	{
		ast = parser.optimize_ast(ast);
		std::cout << peg::ast_to_s(ast);
		std::cout << expr << " = " << eval(*ast) << std::endl;
		return 0;
	}

	std::cerr << "syntax error..." << std::endl;

	return -1;
}