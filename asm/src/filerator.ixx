export module filerator;

export import common;

export namespace cpu
{
	//function that reads a file and returns it's contents
	std::vector<std::uint8_t> filerator(const char* path) noexcept;
}