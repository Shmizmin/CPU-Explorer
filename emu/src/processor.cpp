#include "processor.hpp"
#include "instructions.hpp"
#include "assert.hpp"

#include <thread>
#include <chrono>
#include <iostream>
#include <utility>

#ifdef OVERFLOW
#undef OVERFLOW
#endif

//internally linked (private) functions
namespace
{
	//encodes the possible exception types for the processor
	enum Exception
	{
		UNDEFINED,
		PROTECTION,
	};

	auto raise(::Exception exception) noexcept
	{
		(void)exception;
	}

	//encodes the purpose for each bit in the flags register
	enum Flag : std::size_t
	{
		ZERO      = 0_uz,
		CARRY     = 1_uz,
		OVERFLOW  = 2_uz,
		INTERRUPT = 3_uz,
	};

	//constrains type to be 8 or 16 bit unsigned type
	template<typename T>
	concept is_byte_or_word = std::is_same_v<T, std::uint8_t> or
							  std::is_same_v<T, std::uint16_t>;

	template<typename T1, typename T2> requires std::is_same_v<T1, T2> and ::is_byte_or_word<T1>
	auto do_move(T1& val1, const T2& val2, const std::bitset<8>& flags) noexcept
	{
		//unpack the execution flags specified for the current instruction
		auto is_upper_word = flags.test(0);
		auto is_eight_size = flags.test(1);
		auto is_zero_extra = flags.test(2);

		//verify that only valid prefixes can be used
		dynamic_assert((not is_eight_size and is_zero_extra) or
					   (not is_eight_size and is_upper_word));

		if (!is_eight_size) [[likely]] //is 16-bit
		{
			val1 = val2;

			//3 bytes, 3 cycles
			return std::make_pair(3_u16, 3_u16);
		}

		else [[unlikely]] //is 8-bit
		{
			if (is_upper_word) [[unlikely]] //upper 8-bits
			{
				if (is_zero_extra) [[unlikely]] //zero contents
				{
					val1 = val2;
					val1 &= 0xFF00_u16;
				}

				else //preserve contents
				{
					val1 &= 0x00FF_u16;
				}	val1 |= (val2 & 0xFF00_u16);
			}

			else [[likely]] //lower 8-bits
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
auto cpu::Processor::read8(void) noexcept
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
auto cpu::Processor::read16(void) noexcept
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

//runs the instruction pointed to by the instruction pointer
auto cpu::Processor::execute(std::uint8_t opcode, const std::bitset<8_uz>& flags) noexcept
{
	//unpack the execution flags specified for the current instruction
	auto is_upper_word = flags.test(0);
	auto is_eight_size = flags.test(1);
	auto is_zero_extra = flags.test(2);

	//verify that only valid prefixes can be used
	dynamic_assert(not is_eight_size and is_zero_extra);

	//determine what to do based on that byte
	switch (opcode)
	{
#ifndef PUSH_INSN
#define PUSH_INSN(x)                                             \
		case ::Opcode::PUSH_R##x:                                \
		{                                                        \
			if (!is_eight_size) [[likely]]                       \
			{                                                    \
				/*push rx onto the stack*/                       \
				push16(R##x);                                    \
                                                                 \
				/*1 push, 2 cycles*/                             \
				return std::make_pair(1_u16, 2_u16);             \
			}                                                    \
                                                                 \
			else [[unlikely]]                                    \
			{                                                    \
				/*push rx onto the stack*/                       \
				push8(static_cast<std::uint8_t>(R##x & 0x00FF)); \
                                                                 \
				/*1 push, 1 cycle*/                              \
				return std::make_pair(2_u16, 1_u16);             \
			}                                                    \
		}                                                        \

	   PUSH_INSN(0)
	   PUSH_INSN(1)
	   PUSH_INSN(2)
#undef PUSH_INSN
#else
static_assert(false, "Redefinition of PUSH_INSN");
#endif

#ifndef POP_INSN
#define POP_INSN(x)                                  \
		case ::Opcode::POP_R##x:                     \
		{                                            \
			if (!is_eight_size) [[likely]]           \
			{                                        \
				/*pop into rx from the stack*/       \
				R##x = pop16();                      \
                                                     \
				/*1 pop, 2 cycles*/                  \
				return std::make_pair(1_u16, 2_u16); \
			}                                        \
                                                     \
			else [[unlikely]]                        \
			{                                        \
				/*pop into rx from the stack*/       \
				R##x = pop8();                       \
                                                     \
				/*1 pop, 1 cycle*/                   \
				return std::make_pair(2_u16, 1_u16); \
			}                                        \
		}                                            \

	   POP_INSN(0)
	   POP_INSN(1)
	   POP_INSN(2)
#undef POP_INSN
#else
static_assert(false, "Redefinition of POP_INSN");
#endif

#ifndef POP_JUMP
#define POP_JUMP(x)                              \
		case ::Opcode::x:                        \
		{                                        \
			/*pop an address off of the stack*/  \
			auto target = pop16();               \
                                                 \
			/*jump to that address*/             \
			IP = target;                         \
                                                 \
			/*1 pop, 1 jump, 3 cycles*/          \
			return std::make_pair(1_u16, 3_u16); \
		}                                        \

	   POP_JUMP(INTRET)
	   POP_JUMP(RETURN)
#undef POP_JUMP
#else
static_assert(false, "Redefinition of POP_JUMP");
#endif

#ifndef SET_INTERRUPT
#define SET_INTERRUPT(x, y)                      \
		case ::Opcode::x:                        \
		{                                        \
			/*set and clear the interrupt flag*/ \
			FLAGS[::Flag::INTERRUPT] = y;        \
			return std::make_pair(1_u16, 1_u16); \
		}                                        \

	   SET_INTERRUPT(EI, true)
	   SET_INTERRUPT(DI, false)
#undef SET_INTERRUPT
#else
static_assert(false, "Redefinition of SET_INTERRUPT");
#endif

//#ifndef MOVE_REG_INSN
//#define MOVE_REG_INSN(x, y)                       \
//		case ::Opcode::MOV_##x##_##y:             \
//		{                                         \
//			if (!is_eight_size) [[likely]]        \
//			{                                     \
//				x = y;                            \
//			}                                     \
//                                                  \
//			else [[unlikely]]                     \
//			{                                     \
//				if (is_upper_word) [[unlikely]]   \
//				{                                 \
//					if (is_zero_extra) [[likely]] \
//					{                             \
//						x = y;                    \
//						x &= 0xFF00_u16;          \
//					}                             \
//                                                  \
//					else [[unlikely]]             \
//					{                             \
//						x &= 0x00FF_u16;          \
//						x |= (y & 0xFF00_u16);    \
//					}                             \
//				}                                 \
//                                                  \
//				else [[likely]]                   \
//				{                                 \
//					if (is_zero_extra) [[likely]] \
//					{                             \
//						x = y;                    \
//						x &= 0x00FF_u16;          \
//					}                             \
//                                                  \
//					else [[unlikely]]             \
//					{                             \
//						x &= 0xFF00_u16;          \
//						x |= (y & 0x00FF_u16);    \
//					}                             \
//				}                                 \
//			}                                     \
//                                                  \
//			/*1 move, 1 cycle*/                   \
//			return std::make_pair(1_u16, 1_u16);  \
//		}                                         \
//
//	   MOVE_REG_INSN(R0, R1)
//	   MOVE_REG_INSN(R0, R2)
//	   MOVE_REG_INSN(R1, R0)
//	   MOVE_REG_INSN(R1, R2)
//	   MOVE_REG_INSN(R2, R0)
//	   MOVE_REG_INSN(R2, R1)
//
//#undef MOVE_REG_INSN
//#else
//static_assert(false, "Redefinition of MOVE_REG_INSN");
//#endif

//#ifndef MOVE_IMM_INSN
//#define MOVE_IMM_INSN(x)                                  \
//		case ::Opcode::MOV_##x##_IMM:                     \
//		{                                                 \
//			if (!is_eight_size) [[likely]]                \
//			{                                             \
//				x = ::read16(*this);                      \
//			}                                             \
//                                                          \
//			else [[unlikey]]                              \
//			{                                             \
//				if (is_upper_word) [[unlikely]]           \
//				{                                         \
//					auto val = static_cast<std::uint16_t> \
//							(::read8(*this) << 8);        \
//                                                          \
//					if (is_zero_extra) [[likely]]         \
//					{                                     \
//						x = val;                          \
//					}                                     \
//                                                          \
//					else [[unlikely]]                     \
//					{                                     \
//						x &= 0x00FF_u16;                  \
//						x |= val;                         \
//					}                                     \
//				}                                         \
//		                                                  \
//				else [[unlikely]]                         \
//				{                                         \
//					auto val = static_cast<std::uint16_t> \
//							(::read8(*this) << 0);        \
//                                                          \
//					if (is_zero_extra) [[likely]]         \
//					{                                     \
//						x = val;                          \
//					}                                     \
//                                                          \
//					else [[unlikely]]                     \
//					{                                     \
//						x &= 0xFF00_u16;                  \
//						x |= val;                         \
//					}                                     \
//				}                                         \
//			}                                             \
//		}                                                 \
//
//	   MOVE_IMM_INSN(R0)
//	   MOVE_IMM_INSN(R1)
//	   MOVE_IMM_INSN(R2)
//#undef MOVE_IMM_INSN
//#else
//static_assert(false, "Redefinition of MOVE_IMM_INSN");
//#endif

#define MOV_REG_REG_INSN(x, y)            \
		case ::Opcode::MOV_##x##_##y:     \
			return ::do_move(x, y, flags) \
		
	   MOV_REG_REG_INSN(R0, R1);
	   MOV_REG_REG_INSN(R0, R2);
	   MOV_REG_REG_INSN(R1, R0);
	   MOV_REG_REG_INSN(R1, R2);
	   MOV_REG_REG_INSN(R2, R0);
	   MOV_REG_REG_INSN(R2, R1);
#undef MOV_REG_REG_INSN

#define MOV_REG_IMM_INSN(x)                                         \
		case ::Opcode::MOV_##x##_IMM:                               \
		{                                                           \
			if (is_eight_size) [[unlikely]]                         \
				return ::do_move(x, static_cast<std::uint16_t>      \
					(read8()), flags);                              \
			else [[likely]]                                         \
				return ::do_move(x, read16(), flags);               \
		}                                                           \

		MOV_REG_IMM_INSN(R0);
		MOV_REG_IMM_INSN(R1);
		MOV_REG_IMM_INSN(R2);
#undef MOV_REG_IMM_INSN

#define MOV_REG_MEM_INSN(x)                                    \
		case ::Opcode::MOV_##x##_MEM:                          \
		{                                                      \
			if (is_eight_size) [[unlikely]]                    \
				return ::do_move(x, static_cast<std::uint16_t> \
					(MEM[read8()]), flags);                    \
			else [[likely]]                                    \
				return ::do_move(x, read16(), flags);          \
		}                                                      \

	   MOV_REG_MEM_INSN(R0);
	   MOV_REG_MEM_INSN(R1);
	   MOV_REG_MEM_INSN(R2);
#undef MOV_REG_MEM_INSN
		

		case ::Opcode::POP_DISCARD:
		{
			if (!is_eight_size) [[likely]]
			{
				//pop the stack top 16 bits into nothing
				static_cast<void>(pop16());

				//1 pop, 2 cycles
				return std::make_pair(1_u16, 2_u16);
			}

			else [[unlikely]]
			{
				//pop the stack top 8 bits into nothing
				static_cast<void>(pop8());

				//1 pop, 1 cycle
				return std::make_pair(2_u16, 1_u16);
			}
		}

		case ::Opcode::PUSH_FLAGS:
		{
			//push the flags register
			push16(static_cast<std::uint16_t>(FLAGS.to_ulong()));

			//1 push, 2 cycles
			return std::make_pair(1_u16, 2_u16);
		}

		case ::Opcode::POP_FLAGS:
		{
			//pop the flags register
			FLAGS = { static_cast<unsigned long>(pop16()) };

			//1 pop, 2 cycles
			return std::make_pair(1_u16, 2_u16);
		}

		case ::Opcode::PUSHALL:
		{
			//push the general purpose registers
			push16(R0);
			push16(R1);
			push16(R2);

			//push the specialized registers
			push16(static_cast<std::uint16_t>(FLAGS.to_ulong()));

			//4 pushes, 8 cycles
			return std::make_pair(1_u16, 8_u16);
		}

		case ::Opcode::POPALL:
		{
			//pop the specilized registers
			FLAGS = { static_cast<unsigned long>(pop16()) };

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
	//will store any flags attached to the current instruction
	std::bitset<8> flags{};

	//run the instruction pointed to by the instruction pointer
	auto fetched = read8();

	//determine if the fetched byte was a prefix flag byte
	switch (fetched)
	{
		case 0x97: flags = { 0b000ul }; break;
		case 0x87: flags = { 0b001ul }; break;
		case 0x98: flags = { 0b010ul }; break;
		case 0x88: flags = { 0b011ul }; break;
		case 0xB7: flags = { 0b100ul }; break;
		case 0xA7: flags = { 0b101ul }; break;
		case 0xB8: flags = { 0b110ul }; break;
		case 0xA8: flags = { 0b111ul }; break;
		default:                        break;
	}
	
	if ((flags.test(0) and not flags.test(1)) or
		(flags.test(2) and not flags.test(1)))
			::raise(::Exception::UNDEFINED);

	//execute the instruction with the specified flags
	auto inc = execute(read8(), flags);

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