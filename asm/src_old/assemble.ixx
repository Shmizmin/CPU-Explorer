export module assemble;

export import std.core;
export import std.regex;
export import "literals.hpp";

export namespace cpu
{
	//takes in assembly source and returns raw binary
	std::vector<std::byte> assemble(std::string source) noexcept;
}