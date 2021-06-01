import save;

//saves contents to a file from the specified path
void cpu::save(const std::vector<std::byte>& binary, const std::string& path) noexcept
{
	//create the output filestream from the path
	std::ofstream f(path, std::ios::binary);

	//read the data into the filestream
	for (auto&& b : binary)
		f << std::to_integer<std::uint8_t>(b);
}