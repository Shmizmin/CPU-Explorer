import assemble;

//internally linked functions
namespace
{
	//splits a string into a vector of strings over a delimiter character or string
	//compress flag suppresses duplicate empty strings when multiple delimiters are stacked
	std::vector<std::string> split(std::string text, std::string delim, bool compress)
	{
		std::vector<std::string> vec;
		std::size_t pos = 0_uz, prevPos = 0_uz;
		while (true)
		{
			pos = text.find_first_of(delim, prevPos);

			while (compress)
			{
				if (prevPos == pos)
					prevPos++;
				else
					break;

				pos = text.find_first_of(delim, prevPos);
			}

			if (pos == std::string::npos)
			{
				if (prevPos != text.size())
					vec.emplace_back(text.substr(prevPos));
				return vec;
			}

			vec.emplace_back(text.substr(prevPos, pos - prevPos));
			prevPos = pos + 1_uz;
		}
	}
}

//takes in assembly source and returns raw binary
std::vector<std::byte> cpu::assemble(std::string source) noexcept
{
	const std::vector<std::string> lines = ::split(source, "\n ", true);

	std::vector<std::byte> code;
	code.resize(std::round(code.length() / 2));
	

	return bytes;
}