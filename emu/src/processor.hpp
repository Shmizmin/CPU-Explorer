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
		//halts execution and resets state
		void reset(void) noexcept;

		//run for each clock cycle of the system
		void clock(void) noexcept;
	};
}

#endif