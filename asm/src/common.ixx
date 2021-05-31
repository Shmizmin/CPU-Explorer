export module common;

//standard library imports
export import std.core;
export import std.regex;
export import std.memory;
export import std.threading;

//custom library imports
export import "literals.hpp";
export import "assert.hpp";

//custom library functionality
export namespace cpu
{
	//globally defined constant values
	namespace constants
	{
		//number of bytes of the processor's memory
		static constexpr const auto mem_size = std::numeric_limits<std::uint16_t>::max();

		//number of bits in the processor's registers
		static constexpr const auto reg_size = 8_uz;

		//number of registers that the processor has
		static constexpr const auto reg_count = 8_uz;
	}

	//globally defined debug values
	namespace debug
	{
		//whether or not assembler progress should be printed
		static constexpr const auto display_progress = true;
	}
}