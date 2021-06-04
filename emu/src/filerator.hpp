#ifndef emu_filerator_hpp
#define emu_filerator_hpp

#include <string>

namespace cpu
{
	//loads the contents of a specified file into a string
	std::string filerator(const char* path) noexcept;
}

#endif