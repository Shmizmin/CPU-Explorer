#include <cctype>
#include <fstream>
#include <cassert>
#include <iostream>

#include "driver.hpp"

assembler::Driver::~Driver(void) noexcept
{
	delete(scanner);
	scanner = nullptr;
	delete(parser);
	parser = nullptr;
}

void assembler::Driver::parse(const char* const filename) noexcept
{
	assert(filename != nullptr);
	std::ifstream in_file(filename);
	if (!in_file.good())
	{
		exit(EXIT_FAILURE);
	}
	parse_helper(in_file);
	return;
}

void assembler::Driver::parse(std::istream& stream) noexcept
{
	if (!stream.good() && stream.eof())
	{
		return;
	}

	parse_helper(stream);
	return;
}


void assembler::Driver::parse_helper(std::istream& stream) noexcept
{
	delete(scanner);
	scanner = new assembler::Scanner(&stream);
	delete(parser);
	parser = new assembler::Parser((*scanner), (*this));

	//const int accept(0);
	if (parser->parse() != 0)
	{
		std::cerr << "Parse failed\n";
	}

	return;
}

void assembler::Driver::add_upper(void) noexcept
{
	uppercase++;
	chars++;
	words++;
}

void assembler::Driver::add_lower(void) noexcept
{
	lowercase++;
	chars++;
	words++;
}

void assembler::Driver::add_word(const std::string& word) noexcept
{
	words++;
	chars += word.length();

	for (const char& c : word)
	{
		if (std::islower(static_cast<unsigned char>(c)))
		{
			lowercase++;
		}
		else if (std::isupper(static_cast<unsigned char>(c)))
		{
			uppercase++;
		}
	}
}

void assembler::Driver::add_newline(void) noexcept
{
	lines++;
	chars++;
}

void assembler::Driver::add_char(void) noexcept
{
	chars++;
}


std::ostream& assembler::Driver::print(std::ostream& stream) noexcept
{
	stream << red  << "Results: "    << norm              << "\n";
	stream << blue << "Uppercase: "  << norm << uppercase << "\n";
	stream << blue << "Lowercase: "  << norm << lowercase << "\n";
	stream << blue << "Lines: "      << norm << lines     << "\n";
	stream << blue << "Words: "      << norm << words     << "\n";
	stream << blue << "Characters: " << norm << chars     << "\n";
	return stream;
}