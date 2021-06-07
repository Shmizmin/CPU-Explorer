#include <cstdlib>
#include <iostream>

#include "filerator.hpp"

int __cdecl main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) noexcept
{
	//determine what do based on the number of arguments passed in
	switch (argc)
	{
	//if a single command line argument was passed in
	case 2:
	{
		//retrieve the raw binary data from the file specified
		auto bin = cpu::filerator(argv[1]);

		break;
	}

	//if any other number of arguments was passed in
	default:
		//since an incorrect argument count was passed, throw an error
		std::cerr << "Usage: emu [filepath]\n";
		std::exit(11);
		break;
	}

	//gracefully exit the program after successful assembling
	return 0;
}