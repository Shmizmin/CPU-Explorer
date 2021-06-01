export module filerator;

export import common;

export namespace cpu
{
	//loads the contents of a specified file into a string
	std::string filerator(const char* path) noexcept;
}