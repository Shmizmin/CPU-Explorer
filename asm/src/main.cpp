#include <iostream>
#include <cstdlib>
#include <cstring>
#include "driver.hpp"

auto show_help(void) noexcept
{
	std::cout << "use -o for pipe to std::cin\n";
	std::cout << "just give a filename to count from a file\n";
	std::cout << "use -h to get this menu\n";
	return EXIT_SUCCESS;
}

int __cdecl main(const int argc, const char** argv) noexcept
{
	assembler::Driver driver;

	switch (argc)
	{
		case 2:
		{
			if (std::strncmp(argv[1], "-o", 2) == 0) //input was terminal pipe
				driver.parse(std::cin);
			else //unrecognized
				goto default_case;
		}

		case 3:
		{
			if (std::strncmp(argv[1], "-f", 2) == 0) //input was filepath
				driver.parse(argv[2]);
			else //unrecognized
				goto default_case;
		}

		default:
		{
		default_case:
			show_help();
			break;
		}
	}

	return EXIT_SUCCESS;
}