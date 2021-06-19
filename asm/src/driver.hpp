#ifndef _ASSEMBLER_DRIVER_HPP
#define _ASSEMBLER_DRIVER_HPP

#include <string>
#include <cstddef>
#include <iostream>

#include "scanner.hpp"
#include "parser.tab.hpp"

namespace assembler
{
	class Driver
	{
	public:
		constexpr Driver(void) noexcept = default;

		virtual ~Driver(void) noexcept;

		void parse(const char* filename) noexcept;
		void parse(std::istream& iss) noexcept;

		void add_upper(void) noexcept;
		void add_lower(void) noexcept;
		void add_word(const std::string& word) noexcept;
		void add_newline(void) noexcept;
		void add_char(void) noexcept;

		std::ostream& print(std::ostream& stream) noexcept;

	private:
		void parse_helper(std::istream& stream) noexcept;

		std::size_t chars = 0;
		std::size_t words = 0;
		std::size_t lines = 0;
		std::size_t uppercase = 0;
		std::size_t lowercase = 0;
		assembler::Parser*  parser  = nullptr;
		assembler::Scanner* scanner = nullptr;

		const std::string red  = "\033[1;31m";
		const std::string blue = "\033[1;36m";
		const std::string norm = "\033[0m";
	};

} //namespace assembler
#endif //ifndef _ASSEMBLER_DRIVER_HPP