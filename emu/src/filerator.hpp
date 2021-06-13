#ifndef emu_filerator_hpp
#define emu_filerator_hpp

#include <vector>
#include <cstdint>

namespace cpu
{
	//loads the contents of a specified file into a string
	std::vector<std::uint8_t> filerator(const char* path) noexcept;
}

#endif