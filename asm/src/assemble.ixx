export module assemble;

export import common;

export namespace cpu
{
	//takes in assembly source and returns raw binary
	std::vector<std::byte> assemble(std::string source) noexcept;
}