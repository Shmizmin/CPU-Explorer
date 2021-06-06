#ifndef emu_processor_hpp
#define emu_processor_hpp

#include <cstdint>
#include <bitset>
#include <limits>
#include <array>

#include "literals.hpp"

namespace cpu
{
	struct Processor
	{
	public:
		//flags register
		std::bitset<16_uz> FLAGS{};
		
		//instruction pointer register
		std::uint16_t IP = 0xFFF1;

		//stack pointer register
		std::uint16_t SP = 0xFFF0;

		//general purpose registers
		std::uint16_t R0 = 0x0000,
					  R1 = 0x0000,
					  R2 = 0x0000;

		//data and address 'buses'
		std::uint16_t DB = 0x0000,
					  AB = 0x0000;

		//entire address space
		std::array<std::uint8_t, std::numeric_limits<std::uint16_t>::max()> MEM{};

	public:
		constexpr Processor() noexcept
		{
		}

		constexpr ~Processor() noexcept
		{
		}

	public:
		//read 8 bits of information from memory
		auto read8(void) noexcept;
		
		//write 8 bits of information to memory
		auto write8(std::uint8_t, std::uint16_t) noexcept;

		//read 16 bits of information from memory
		auto read16(void) noexcept;

		//write 16 bits of information to memory
		auto write16(std::uint16_t, std::uint16_t) noexcept;

		//push 8 bits of information to the stack
		auto push8(std::uint8_t) noexcept;

		//pop 8 bits of information from the stack
		auto pop8(void) noexcept;

		//push 16 bits of information to the stack
		auto push16(std::uint16_t) noexcept;

		//pop 16 bits of information from the stack
		auto pop16(void) noexcept;

		//runs the instruction pointed to by the instruction pointer
		auto execute(std::uint8_t, const std::bitset<8_uz>&) noexcept;

		//runs an entire instruction sequence
		auto run(void) noexcept;

		//runs for each clock cycle of the system
		auto clock(void) noexcept;
	};
}

#endif