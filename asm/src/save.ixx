export module save;

export import std.core;

export namespace cpu
{
	//saves contents to a file from the specified path
	void save(const std::vector<std::byte>& binary, const std::string& path) noexcept;
}