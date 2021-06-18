#ifndef _ASSEMBLER_SCANNER_HPP
#define _ASSEMBLER_SCANNER_HPP

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "parser.tab.hpp"
#include "location.hh"

namespace assembler
{
	class Scanner : public yyFlexLexer
	{
	public:
		Scanner(std::istream* in)
			: yyFlexLexer(in)
		{
		}

		virtual ~Scanner(void) noexcept
		{
		}

		using FlexLexer::yylex;

		virtual int yylex(assembler::Parser::semantic_type* const lval,
						  assembler::Parser::location_type* location);

	private:
		assembler::Parser::semantic_type* yylval = nullptr;
	};

} //namespace assembler

#endif //ifndef _ASSEMBLER_SCANNER_HPP