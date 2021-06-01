import common;
import filerator;
import assemble;
import save;

#if !defined _M_X64
static_assert(false, "This program can only be compiled for 64-bit machines");
#endif

int __cdecl main(int argc, [[maybe_unused]] char** argv) noexcept
{
	//determine what do based on the number of arguments passed in
	switch (argc)
	{
	//if a single command line argument was passed in
	case 2:
	{
		//retrieve the filepath passed in from the arguments
		auto& path = argv[1];
	
		//load up the contents of the indicated file
		auto file = cpu::filerator(path);

		//assemble the file contents
		auto bin = cpu::assemble(file);

		//save the assembled binary to disk
		auto out = std::regex_replace(path, std::regex(R"((.+\.)asm)"), "$1bin");
		cpu::save(bin, out);

		break;
	}

	//if any other number of arguments was passed in
	default:
		//since an incorrect argument count was passed, throw an error
		std::cerr << "Usage: asm [filepath]\n";
		std::exit(11);
		break;
	}

	//gracefully exit the program after successful assembling
	return 0;
}