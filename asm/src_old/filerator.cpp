import filerator;

//loads the contents of a specified file into a string
std::string cpu::filerator(const char* path) noexcept
{
	//open the stream to load the file
	std::ifstream f(path);

	//retrieve the size of the file
	f.ignore(std::numeric_limits<std::streamsize>::max());
	auto bytes = f.gcount();
	f.seekg(0, std::ios::beg);

	//create a copy buffer
	std::string result;
	result.resize(bytes);

	//read the file into the buffer
	result.assign(std::istreambuf_iterator<char>(f),
				  std::istreambuf_iterator<char>());

	//return the newly created buffer
	return result;
}