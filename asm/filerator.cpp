import filerator;

//function that reads a file and returns it's contents
std::vector<std::uint8_t> cpu::filerator(const char* path) noexcept
{
	//create the filestream object from the specified path
	std::ifstream f(path, std::ios::binary);

	//verify that the file in the filestream is readable
	if (!f.is_open()) [[unlikely]]
	{
		std::cerr << "The specified file does not exist";
		std::exit(10);
	}

	//retrieve the length of the files in bytes
	f.seekg(0, std::ios::end);
	auto bytes = f.tellg();
	f.seekg(0, std::ios::beg);

	//create a file buffer of the same size as the file itself
	std::vector<std::uint8_t> buffer(bytes);

	//copy the contents of the file to the file buffer
	buffer.assign(std::istreambuf_iterator<char>(f),
				  std::istreambuf_iterator<char>());

	//return the contents of the file buffer from this function
	return buffer;
}