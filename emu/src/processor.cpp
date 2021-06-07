#include "processor.hpp"
#include "instructions.hpp"
#include "assert.hpp"

#include <type_traits>
#include <thread>
#include <chrono>
#include <iostream>
#include <utility>
#include <bit>

#ifdef OVERFLOW
#undef OVERFLOW
#endif


//internally linked (private) functions
namespace
{
	/*
	auto do_move(std::uint16_t& val1, const std::uint16_t& val2, const std::bitset<8>& flags, cpu::Processor& cpu) noexcept
	{
		if (!is_eight_size) [[likely]] //is 16-bit
		{
			val1 = val2;

			//3 bytes, 3 cycles
			return std::make_pair(3_u16, 3_u16);
		}

		else [[unlikely]] //is 8-bit
		{
			if (is_upper_word and is_upper_byte)
			{
				if (is_zero_extra) [[unlikely]] //zero contents
				{
					val1 = upper2;
				}

				else [[likely]] //preserve contents
				{
					val1 = (upper2 | lower1);
				}
			}

			else if (is_upper_word and not is_upper_byte)
			{
				if (is_zero_extra) [[unlikely]] //zero contents
				{
					val1 = 0_u16;
					val1 |= (val2 << 8);
				}

				else [[likely]] //preserve contents
				{
					val1 &= 0x00FF_u16;
					val1 = (val21)
					val1 |= (val2 << 8);
				}
			}

			else if (not is_upper_word and is_upper_byte)
			{
				if (is_zero_extra) [[unlikely]] //zero contents
				{
					val1 = 0_uz;
					val1 |= (val2 >> 8);
				}

				else [[likely]] //preserve contents
				{
					val1 &= 0xFF00_u16;
					val1 |= (val2 >> 8);
				}
			}

			else if (not is_upper_word and not is_upper_byte)
			{
				if (is_zero_extra) [[unlikely]] //zero contents
				{
					val1 = val2;
					val1 &= 0x00FF_u16;
				}

				else [[likely]] //preserve contents
				{
					val1 &= 0xFF00_u16;
					val1 |= (val2 & 0x00FF_u16);
				}
			}

			//2 bytes, 2 cycles
			return std::make_pair(2_u16, 2_u16);
		}
	}
	*/

	/*
	auto do_add(std::uint16_t& val1, const std::uint16_t& val2, const std::bitset<8>& flags) noexcept
	{
		UNPACK_FLAGS

		if (!is_eight_size) [[likely]] //is 16-bit
		{
			val1 += val2;

			//3 bytes, 3 cycles
			return std::make_pair(3_u16, 3_u16);
		}

		else [[unlikely]] //is 8-bit
		{
			if (is_upper_word and is_upper_byte)
			{
				if (is_zero_extra) [[unlikely]]
				{
					//val1 &= 0xFF00_u16;
					//val1 += (val2 & 0xFF00_u16);

					val1 >>= 8_u16;
					val1 += ((val2 & 0xFF00_u16) >> 8_u16);
					if (val1 == 0_u16) FLAGS[::Flag::ZERO] = true;
					val1 <<= 8_u16;
				}

				else [[likely]]
				{
					auto top =  ((val1 & 0xFF00_u16) >> 8_u16);
						 top += ((val2 & 0xFF00_u16) >> 8_u16);
					val1 &= 0x00FF_u16;
					val1 |= (top << 8);
				}
			}

			else if (is_upper_word and not is_upper_byte)
			{
				if (is_zero_extra) [[unlikely]]
				{
					val1 &= 0xFF00_u16;
					val1 +=
				}

				else [[likely]]
				{

				}
			}

			else if (not is_upper_word and is_upper_byte)
			{

			}

			else if (not is_upper_word and not is_upper_byte)
			{

			}

			//2 bytes, 2 cycles
			return std::make_pair(2_u16, 3_u16);
		}
	}
	*/

	//composes two bytes into a word
	auto compose(std::uint8_t lower, std::uint8_t upper) noexcept
	{
		return static_cast<std::uint16_t>((static_cast<std::uint16_t>(lower) << 0) |
										  (static_cast<std::uint16_t>(upper) << 8));
	}

	//decomposes a word into two bytes
	auto decompose(std::uint16_t word) noexcept
	{
		return std::make_pair(static_cast<std::uint8_t>((word & 0xFF00) >> 8),
							  static_cast<std::uint8_t>((word & 0x00FF) >> 0));
	}
}

//read 8 bits of information from memory
auto& cpu::Processor::read8(void) noexcept
{
	DB = static_cast<std::uint16_t>(MEM[AB]);
	return MEM[AB];
}

//write 8 bits of information to memory
auto cpu::Processor::write8(std::uint8_t val, std::uint16_t addr) noexcept
{
	AB = addr;
	DB = val;
	MEM[AB] = static_cast<std::uint8_t>(DB);
}

//read 16 bits of information from memory
auto& cpu::Processor::read16(void) noexcept
{
	//read in the upper and lower halves
	auto upper = read8();
	++AB;
	auto lower = read8();

	//compose them into a single value and return
	DB = ::compose(lower, upper);
	return DB;
}

//write 16 bits of information to memory
auto cpu::Processor::write16(std::uint16_t val, std::uint16_t addr) noexcept
{
	//get the upper and lower halves of the value
	auto [upper, lower] = ::decompose(val);

	//write out each byte
	write8(upper, addr);
	write8(lower, addr);
}

//push 8 bits of information to the stack
auto cpu::Processor::push8(std::uint8_t val) noexcept
{
	//reserve a slot on the stack
	--SP;

	//write the data in that slot
	AB = SP;
	DB = val;
	MEM[AB] = static_cast<std::uint8_t>(DB);
}

//pop 8 bits of information from the stack
auto cpu::Processor::pop8(void) noexcept
{
	//deallocate the stack slot
	++SP;

	//read the data
	AB = SP - 1_u16;
	DB = MEM[AB];
	return static_cast<std::uint8_t>(DB);
}

//push 16 bits of information to the stack
auto cpu::Processor::push16(std::uint16_t val) noexcept
{
	//extract the 8-bit upper and lower halves
	auto [upper, lower] = ::decompose(val);

	//write both segments to the stack
	push8(lower);
	push8(upper);
}

//pop 16 bits of information from the stack
auto cpu::Processor::pop16(void) noexcept
{
	//read in the upper and lower halves
	auto upper = pop8();
	auto lower = pop8();

	//compose those into a single word
	return ::compose(lower, upper);
}

//general instruction implementation
#define GNRL_INSN_IMPL(x, y, z)                                                                  \
	if (!EF.SIZE_OVERRIDE) [[likely]]                                                            \
	{                                                                                            \
		dst.ALL x src.ALL;                                                                       \
                                                                                                 \
		/*1 cycle*/                                                                              \
		return std::make_pair(1_u16, y##_u16);                                                   \
	}                                                                                            \
                                                                                                 \
	else [[unlikely]]                                                                            \
	{                                                                                            \
		if (EF.IS_UPPER_DEST and EF.IS_UPPER_SRCE) [[unlikely]] /*high src, high dest*/          \
		{                                                                                        \
			if (EF.ZERO_EXTENDED) [[unlikely]]                                                   \
			{                                                                                    \
				dst.ALL = 0_u16;                                                                 \
				dst.HI x src.HI;                                                                 \
			}                                                                                    \
                                                                                                 \
			else [[likely]]                                                                      \
			{                                                                                    \
				dst.HI x src.HI;                                                                 \
			}                                                                                    \
		}                                                                                        \
                                                                                                 \
		else if (EF.IS_UPPER_DEST and not EF.IS_UPPER_SRCE) [[unlikely]] /*low src, high dest*/  \
		{                                                                                        \
			if (EF.ZERO_EXTENDED) [[unlikely]]                                                   \
			{                                                                                    \
				dst.ALL = 0_u16;                                                                 \
				dst.HI x src.LO;                                                                 \
			}                                                                                    \
                                                                                                 \
			else [[likely]]                                                                      \
			{                                                                                    \
				dst.HI x src.LO;                                                                 \
			}                                                                                    \
		}                                                                                        \
                                                                                                 \
		else if (not EF.IS_UPPER_DEST and EF.IS_UPPER_SRCE) [[unlikely]] /*high src, low dest*/  \
		{                                                                                        \
			if (EF.ZERO_EXTENDED) [[unlikely]]                                                   \
			{                                                                                    \
				dst.ALL = 0_u16;                                                                 \
				dst.LO x src.HI;                                                                 \
			}                                                                                    \
                                                                                                 \
			else [[likely]]                                                                      \
			{                                                                                    \
				dst.LO x src.HI;                                                                 \
			}                                                                                    \
		}                                                                                        \
                                                                                                 \
		else if (not EF.IS_UPPER_DEST and not EF.IS_UPPER_SRCE) [[likely]] /*low src, low dest*/ \
		{                                                                                        \
			if (EF.ZERO_EXTENDED) [[unlikely]]                                                   \
			{                                                                                    \
				dst.ALL = 0_u16;                                                                 \
				dst.LO x src.LO;                                                                 \
			}                                                                                    \
                                                                                                 \
			else [[likely]]                                                                      \
			{                                                                                    \
				dst.LO x src.LO;                                                                 \
			}                                                                                    \
		}                                                                                        \
                                                                                                 \
		/*1 cycle*/                                                                              \
		return std::make_pair(2_u16, z##_u16);                                                   \
	}                                                                                            \

//move instruction implementaton
auto cpu::Processor::do_move_insn(cpu::Register& dst, const cpu::Register& src) noexcept
{
	GNRL_INSN_IMPL(=, 2, 1)
}

//add instruction implementaton
auto cpu::Processor::do_add_insn(cpu::Register& dst, const cpu::Register& src) noexcept
{
	GNRL_INSN_IMPL(+=, 3, 2)
}

//sub instruction implementation
auto cpu::Processor::do_sub_insn(cpu::Register& dst, const cpu::Register& src) noexcept
{
	GNRL_INSN_IMPL(-=, 3, 2)
}

//xor instruction implementation
auto cpu::Processor::do_xor_insn(cpu::Register& dst, const cpu::Register& src) noexcept
{
	GNRL_INSN_IMPL(^=, 3, 2)
}

//or instruction implementation
auto cpu::Processor::do_or_insn(cpu::Register& dst, const cpu::Register& src) noexcept
{
	GNRL_INSN_IMPL(|=, 3, 2)
}

//and instruction implementation 
auto cpu::Processor::do_and_insn(cpu::Register& dst, const cpu::Register& src) noexcept
{
	GNRL_INSN_IMPL(&=, 3, 2)
}


//runs the instruction pointed to by the instruction pointer
auto cpu::Processor::execute(std::uint8_t opcode) noexcept
{
	//determine what to do based on that byte
	switch (opcode)
	{
#define GNRL_REG_REG_INSN(x, y, z, w)   \
		case ::Opcode::w##_##x##_##y:   \
			return do_##z##_insn(x, y); \

	   GNRL_REG_REG_INSN(R0, R1, move, MOV)
	   GNRL_REG_REG_INSN(R0, R2, move, MOV)
	   GNRL_REG_REG_INSN(R1, R0, move, MOV)
	   GNRL_REG_REG_INSN(R1, R2, move, MOV)
	   GNRL_REG_REG_INSN(R2, R0, move, MOV)
	   GNRL_REG_REG_INSN(R2, R1, move, MOV)

	   GNRL_REG_REG_INSN(R0, R1, add, ADD)
	   GNRL_REG_REG_INSN(R0, R2, add, ADD)
	   GNRL_REG_REG_INSN(R1, R0, add, ADD)
	   GNRL_REG_REG_INSN(R1, R2, add, ADD)
	   GNRL_REG_REG_INSN(R2, R0, add, ADD)
	   GNRL_REG_REG_INSN(R2, R1, add, ADD)

	   GNRL_REG_REG_INSN(R0, R1, sub, SUB)
	   GNRL_REG_REG_INSN(R0, R2, sub, SUB)
	   GNRL_REG_REG_INSN(R1, R0, sub, SUB)
	   GNRL_REG_REG_INSN(R1, R2, sub, SUB)
	   GNRL_REG_REG_INSN(R2, R0, sub, SUB)
	   GNRL_REG_REG_INSN(R2, R1, sub, SUB)
#undef GNRL_REG_REG_INSN

//#ifndef PUSH_INSN
//#define PUSH_INSN(x, y)                                          \
//		case ::Opcode::PUSH_R##x:                                \
//		{                                                        \
//			if (!EF.SIZE_OVERRIDE) [[likely]]                    \
//			{                                                    \
//				push16(R##x##y);                                 \
//                                                                 \
//				/*1 push, 2 cycles*/                             \
//				return std::make_pair(1_u16, 2_u16);             \
//			}                                                    \
//                                                                 \
//			else [[unlikely]]                                    \
//			{                                                    \
//				push8(static_cast<std::uint8_t>(R##x & 0x00FF)); \
//                                                                 \
//				/*1 push, 1 cycle*/                              \
//				return std::make_pair(2_u16, 1_u16);             \
//			}                                                    \
//		}                                                        \
//
//	   PUSH_INSN(0, .ALL)
//	   PUSH_INSN(1, .ALL)
//	   PUSH_INSN(2, .ALL)
//#undef PUSH_INSN
//#else
//static_assert(false, "Redefinition of PUSH_INSN");
//#endif
//
//#ifndef POP_INSN
//#define POP_INSN(x)                                  \
//		case ::Opcode::POP_R##x:                     \
//		{                                            \
//			if (!EF.SIZE_OVERRIDE) [[likely]]        \
//			{                                        \
//				R##x = pop16();                      \
//													 \
//				/*1 pop, 2 cycles*/                  \
//				return std::make_pair(1_u16, 2_u16); \
//			}                                        \
//													 \
//			else [[unlikely]]                        \
//			{                                        \
//				R##x = pop8();                       \
//													 \
//				/*1 pop, 1 cycle*/                   \
//				return std::make_pair(2_u16, 1_u16); \
//			}                                        \
//		}                                            \
//
//	   POP_INSN(0)
//	   POP_INSN(1)
//	   POP_INSN(2)
//#undef POP_INSN
//#else
//static_assert(false, "Redefinition of POP_INSN");
//#endif
//
//#ifndef POP_JUMP
//#define POP_JUMP(x)                              \
//		case ::Opcode::x:                        \
//		{                                        \
//			IP = pop16();                        \
//                                                 \
//			/*1 pop, 1 jump, 3 cycles*/          \
//			return std::make_pair(1_u16, 3_u16); \
//		}                                        \
//
//	   POP_JUMP(INTRET)
//	   POP_JUMP(RETURN)
//#undef POP_JUMP
//#else
//static_assert(false, "Redefinition of POP_JUMP");
//#endif

#ifndef SET_INTERRUPT
#define SET_INTERRUPT(x, y)                      \
		case ::Opcode::x:                        \
		{                                        \
			/*set and clear the interrupt flag*/ \
			SF.IF = y;                           \
			return std::make_pair(1_u16, 1_u16); \
		}                                        \

	   SET_INTERRUPT(EI, true)
	   SET_INTERRUPT(DI, false)
#undef SET_INTERRUPT
#else
static_assert(false, "Redefinition of SET_INTERRUPT");
#endif


		case ::Opcode::POP_DISCARD:
		{
			if (EF.SIZE_OVERRIDE) [[likely]]
			{
				static_cast<void>(pop16());

				//1 pop, 2 cycles
				return std::make_pair(1_u16, 2_u16);
			}

			else [[unlikely]]
			{
				static_cast<void>(pop8());

				//1 pop, 1 cycle
				return std::make_pair(2_u16, 1_u16);
			}
		}

		case ::Opcode::PUSH_FLAGS:
		{
			//push the flags register
			std::uint16_t reg;
			std::memcpy(&reg, &SF, sizeof(std::uint16_t));
			push16(reg);

			//1 push, 2 cycles
			return std::make_pair(1_u16, 2_u16);
		}

		case ::Opcode::POP_FLAGS:
		{
			//pop the flags register
			std::uint16_t flags = pop16();
			std::memcpy(&SF, &flags, sizeof(std::uint16_t));

			//1 pop, 2 cycles
			return std::make_pair(1_u16, 2_u16);
		}

		case ::Opcode::PUSHALL:
		{
			//save the general purpose registers
			push16(R0.ALL);
			push16(R1.ALL);
			push16(R2.ALL);

			//save the status flags register
			std::uint16_t reg;
			std::memcpy(&reg, &SF, sizeof(std::uint16_t));
			push16(reg);

			//4 pushes, 8 cycles
			return std::make_pair(1_u16, 8_u16);
		}

		case ::Opcode::POPALL:
		{
			//pop the status flags register
			std::uint16_t flags = pop16();
			std::memcpy(&SF, &flags, sizeof(std::uint16_t));

			//pop the general purpose registers
			R2 = pop16();
			R1 = pop16();
			R0 = pop16();

			//4 pops, 8 cycles
			return std::make_pair(1_u16, 8_u16);
		}

		case ::Opcode::SWINT:
		{
			//save the current instruction pointer
			push16(IP);

			//jump to the software interrupt vector and read it
			AB = 0xFFF3;
			auto target = read16();

			//jump to the specified interrupt handler
			IP = target;

			//1 push, 1 read, 2 jumps, 6 cycles
			return std::make_pair(1_u16, 6_u16);
		}

		case ::Opcode::RESET:
		{
			//put known good values in each register
			R0 = 0x0000,
			R1 = 0x0000,
			R2 = 0x0000,
			SP = 0xFFF0;

			//set the instruction pointer to the reset vector
			IP = 0xFFF1;

			//jump to the address pointed to by the reset vector
			IP = read16();

			//1 read, 2 jumps, 4 cycles
			return std::make_pair(1_u16, 4_u16);
		}
	}

	
}

//runs an entire instruction sequence
auto cpu::Processor::run(void) noexcept
{
	//save a local, modifiable copy of the instruction pointer
	auto IP_COPY = this->IP;

	//read the first byte of the instruction
	auto opcode = read8();
}

//run for each clock cycle of the system
auto cpu::Processor::clock(void) noexcept
{
	//run the instruction pointed to by the instruction pointer
	auto fetched = read8();

	//determine if the fetched byte was a prefix flag byte
	switch (fetched)
	{
		//aggregate initialization for bitfields order:
		//IS_UPPER_SRCE, IS_UPPER_DEST, ZERO_EXTENDED, SIZE_OVERRIDE
		case 0x87: EF = { 0, 0, 0, 0 }; break;
		case 0x88: EF = { 0, 0, 1, 0 }; break;
		case 0x89: EF = { 0, 0, 0, 1 }; break;
		case 0x8A: EF = { 0, 0, 1, 1 }; break;

		case 0x97: EF = { 1, 0, 0, 0 }; break;
		case 0x98: EF = { 1, 0, 1, 0 }; break;
		case 0x99: EF = { 1, 0, 0, 1 }; break;
		case 0x9A: EF = { 1, 0, 1, 1 }; break;

		case 0xA7: EF = { 0, 1, 0, 0 }; break;
		case 0xA8: EF = { 0, 1, 1, 0 }; break;
		case 0xA9: EF = { 0, 1, 0, 1 }; break;
		case 0xAA: EF = { 0, 1, 1, 1 }; break;

		case 0xB7: EF = { 1, 1, 0, 0 }; break;
		case 0xB8: EF = { 1, 1, 1, 0 }; break;
		case 0xB9: EF = { 1, 1, 0, 1 }; break;
		case 0xBA: EF = { 1, 1, 1, 1 }; break;

		default:                                     break;
	}

	//execute the instruction with the specified flags
	auto inc = execute(read8());

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