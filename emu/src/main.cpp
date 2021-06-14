#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

#include "filerator.hpp"
#include "processor.hpp"

template<bool debug = false>
inline const auto basic_exec(const char* const filepath) noexcept
{
	//retrieve the raw binary data from the file specified
	auto bin = cpu::filerator(filepath);

	//create the processor object
	auto processor = std::make_unique<cpu::Processor>(std::move(bin));

	//configure the processor state according to invokation requests
	processor->debugging = debug;

	//set the processor to a known state
	processor->reset();

	//begin executing the program
	processor->execute();
}


int __cdecl main(int argc, const char** argv) noexcept
{
	//determine what do based on the number of arguments passed in
	switch (argc)
	{
	//if a single command line argument was passed in
	case 2:
	{
		//obtain the filepath specified from the command line
		const auto& filepath = argv[1];

		//execute the contents of the binary file
		::basic_exec<false>(filepath);

		break;
	}

	//if two command line arguments were passed in
	case 3:
	{
		//obtain the arguments from the command line
		//program expects
		const auto& filepath = argv[2],
					runflags = argv[1];

		if (std::strcmp(runflags, "-d") == 0)
			::basic_exec<true>(filepath);

		else
		{
			std::cerr << "" << std::endl;
			std::exit(17);
		}
	}

	//if any other number of arguments was passed in
	default:
		//since an incorrect argument count was passed, throw an error
		std::cerr << "Usage: emulate [flags] [filepath]\n";
		std::exit(11);
		break;
	}

	//gracefully exit the program after successful assembling
	return 0;
}