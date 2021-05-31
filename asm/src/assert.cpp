import common;

//function that performs a debug assertion exit
void ::_assert(char const* message, const char* function, char const* filename, unsigned line)
{
	std::cerr << "\a[" << function << "] Debug assertion failed! Faulting expression : " << message << '\n';
	std::cerr << "File: " << filename << " on line " << line << '\n';
	std::exit(line);
}