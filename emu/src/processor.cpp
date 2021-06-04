#include "processor.hpp"
#include "instructions.hpp"

#include <thread>
#include <chrono>
#include <iostream>
#include <utility>

//internally linked (private) functions
namespace
{
	//encodes the purpose for each bit in the flags register
	enum Flag : std::size_t
	{
		FLAG_ZERO      = 0_uz,
		FLAG_CARRY     = 1_uz,
		FLAG_OVERFLOW  = 2_uz,
		FLAG_INTERRUPT = 3_uz,
	};

	//composes two bytes into a word
	auto compose(std::uint8_t lower, std::uint8_t upper) noexcept
	{
		return static_cast<std::uint16_t>(static_cast<std::uint16_t>(lower) |
										 (static_cast<std::uint16_t>(upper) << 8));
	}

	//pushes a single byte to the stack
	auto push8(cpu::Processor& cpu, std::uint8_t val) noexcept
	{
		//reserve a slot on the stack
		--cpu.SP;

		//write the data on
		cpu.MEM[cpu.SP] = val;
	}

	//pushes a single word to the stack
	auto push16(cpu::Processor& cpu, std::uint16_t val) noexcept
	{
		//extract the 8-bit upper and lower halves
		auto upper = static_cast<std::uint8_t>((val & 0xFF00) >> 8);
		auto lower = static_cast<std::uint8_t>((val & 0x00FF) >> 0);

		//write both segments to the stack
		::push8(cpu, lower);
		::push8(cpu, upper);
	}

	//pops a single byte off of the stack
	auto pop8(cpu::Processor& cpu) noexcept
	{
		//deallocate the stack slot
		++cpu.SP;

		//read the data
		return cpu.MEM[cpu.SP - 1_u16];
	}

	//pops a single word off of the stack
	auto pop16(cpu::Processor& cpu) noexcept
	{
		//read in the upper and lower halves
		auto upper = ::pop8(cpu);
		auto lower = ::pop8(cpu);

		//compose those into a single word
		return ::compose(lower, upper);
	}

	//reads a single byte from memory pointed to by the instruction pointer
	template<bool inc = true>
	auto read8(cpu::Processor& cpu) noexcept
	{
		//if instruction pointer auto increment was enabled
		if constexpr (inc)
		{
			//increment the instruction pointer
			++cpu.IP;

			//read the data
			return cpu.MEM[cpu.IP - 1_u16];
		}

		//if instruction pointer auto increment was disabled
		else
		{
			//read the data
			return cpu.MEM[cpu.IP];
		}
	}

	//reads a single word from memory pointed to by the instruction pointer
	template<bool inc = true>
	auto read16(cpu::Processor& cpu) noexcept
	{
		//read in the upper and lower halves
		auto upper = ::read8<inc>(cpu);
		if constexpr (!inc) ++cpu.IP;
		auto lower = ::read8<inc>(cpu);
		if constexpr (!inc) --cpu.IP;

		//compose those into a single word
		return ::compose(lower, upper);
	}

	//runs the instruction pointed to by the instruction pointer
	auto execute(cpu::Processor& cpu) noexcept
	{
		//save a local, modifiable copy of the instruction pointer
		auto IP = cpu.IP;

		//read the first byte of the instruction
		auto opcode = ::read8<false>(cpu);

		//determine what to do based on that byte
		switch (opcode)
		{
			case ::Opcode::PUSH_FLAGS:
			{
				//push the flags register
				::push16(cpu, static_cast<std::uint16_t>(cpu.FLAGS.to_ulong()));

				//1 push, 1 cycle
				return std::pair{ 1_u16, 1_u16 };
			}

			case ::Opcode::POP_FLAGS:
			{
				//pop the flags register
				cpu.FLAGS = { static_cast<unsigned long>(::pop16(cpu)) };

				//1 pop, 1 cycle
				return std::pair{ 1_u16, 1_u16 };
			}

			case ::Opcode::PUSHALL:
			{
				//push the general purpose registers
				::push16(cpu, cpu.R0);
				::push16(cpu, cpu.R1);
				::push16(cpu, cpu.R2);

				//push the specialized registers
				::push16(cpu, static_cast<std::uint16_t>(cpu.FLAGS.to_ulong()));

				//4 pushes, 4 cycles
				return std::pair{ 1_u16, 4_u16 };
			}

			case ::Opcode::POPALL:
			{
				//pop the specilized registers
				cpu.FLAGS = { static_cast<unsigned long>(::pop16(cpu)) };

				//pop the general purpose registers
				cpu.R2 = ::pop16(cpu);
				cpu.R1 = ::pop16(cpu);
				cpu.R0 = ::pop16(cpu);

				//4 pops, 4 cycles
				return std::pair{ 1_u16, 4_u16 };
			}

			case ::Opcode::EI:
			{
				//set the interrupt flag directly
				cpu.FLAGS[::Flag::FLAG_INTERRUPT] = true;
				return std::pair{ 1_u16, 1_u16 };
			}

			case ::Opcode::DI:
			{
				//clear the interrupt flag directly
				cpu.FLAGS[::Flag::FLAG_INTERRUPT] = false;
				return std::pair{ 1_u16, 1_u16 };
			}

			case ::Opcode::SIZE16TO8:
			{
				//do something that prefix overrides the following
				//instruction's operand size from 16 to 8 bits
			}
		}
	}
}

//halts execution and resets state
void cpu::Processor::reset(void) noexcept
{
	//put known good values in each register
	R0 = 0x0000,
	R1 = 0x0000,
	R2 = 0x0000,
	SP = 0xFFF0;

	//set the instruction pointer to the reset vector
	IP = 0xFFF1;

	//jump to the address pointed to by the reset vector
	IP = ::read16<true>(*this);
}

//run for each clock cycle of the system
void cpu::Processor::clock(void) noexcept
{
	//run the instruction pointed to by the instruction pointer
	auto inc = ::execute(*this);

	//increment the instruction pointer
	IP += inc.first;

	//retrieve the requested execution speed
	auto speed = ::compose(MEM[0xFFFC], MEM[0xFFFB]);

	/*Verify that we don't do a division by zero, but as a good
	side effect, we can zero out the speed register and get different
	behavior. In our case, we use this to indiciate single step mode.*/
	if (speed == 0) [[unlikely]]
	{
		//something about single step mode
		std::cin.get();
	}

	//otherwise, wait
	else [[likely]]
	{
		/*The speed register indicates approximate execution frequency,
		so to wait for the correct amount of time, we must take the inverse*/
		std::this_thread::sleep_for(std::chrono::duration<double, std::milli>
			{ (1.0 / static_cast<double>(speed * inc.second)) });
	}
}