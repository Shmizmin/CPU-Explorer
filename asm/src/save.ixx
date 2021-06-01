export module save;

export import common;


export namespace cpu
{
	//saves contents to a file from the specified path
	void save(const std::vector<std::byte>& binary, const std::string& path) noexcept;
}