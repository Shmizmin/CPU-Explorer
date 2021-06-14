#include "processor.hpp"
#include "instructions.hpp"
#include "assert.hpp"

#include <unordered_map>
#include <type_traits>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <bit>

#pragma region InternalFunction
//internally linked (private) functions
namespace
{
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

	//outputs the necessary processor status information so it can be debugged externally
	auto debug(cpu::Processor* cpu) noexcept
	{

	}

	enum class Addressing
	{
		//all register 0 on register operations
		R0_R0,
		R0_R1,
		R0_R2,

		//all register 1 on register operations
		R1_R0,
		R1_R1,
		R1_R2,

		//all register 2 on register operations
		R2_R0,
		R2_R1,
		R2_R2,

		//all register on immediate data operations
		R0_IMM,
		R1_IMM,
		R2_IMM,

		//all register on memory operations
		R0_MEM,
		R1_MEM,
		R2_MEM,

		//all memory on register operations
		MEM_R0,
		MEM_R1,
		MEM_R2,

		//basic unary operations
		R0,
		R1,
		R2,
		IMM,
		MEM,

		//special unary operations
		SP,
		IP,
		FLAGS,

		//all stack pointer operations
		SP_R0,
		SP_R1,
		SP_R2,
		SP_IMM,
		SP_MEM,

		//other special cases
		DISC,
		LABEL,
		NONE,
	};

#define O(x, y, z, u, v)                                                                                         \
	{ x##_##y, { static_cast<std::uint8_t>(z), static_cast<std::uint8_t>(u), static_cast<std::uint8_t>(v), y } } \

	//map: { opcode } -> tuple{ operand count, byte count 16size, byte count 8size }
	std::unordered_map<std::uint8_t, std::tuple<std::uint8_t, std::uint8_t, std::uint8_t, ::Addressing>> opcodes
	{
		O(MOV, R0_R1,  2, 1, 2), O(ADD, R0_R1,  2, 1, 2), O(SUB, R0_R1,  2, 1, 2), O(XOR, R0_R1,  2, 1, 2), O(OR,   R0_R1,  2, 1, 2), O(AND,   R0_R1,  2, 1, 2), O(CMP,  R0_R1,  2, 1, 2),                          O(PUSH, R0,    1, 1, 1), O(POP, R0,    1, 1, 1), O(ADD, SP_R0,  2, 1, 1), O(SUB, SP_R0,  2, 1, 1), O(ROR,     R0_R1,  2, 1, 2), O(ROL,    R0_R1,  2, 1, 2), O(SHR, R0_R1,  2, 1, 2), O(SHL, R0_R1,  2, 1, 2),
		O(MOV, R0_R2,  2, 1, 2), O(ADD, R0_R2,  2, 1, 2), O(SUB, R0_R2,  2, 1, 2), O(XOR, R0_R2,  2, 1, 2), O(OR,   R0_R2,  2, 1, 2), O(AND,   R0_R2,  2, 1, 2), O(CMP,  R0_R2,  2, 1, 2),                          O(PUSH, R1,    1, 1, 1), O(POP, R1,    1, 1, 1), O(ADD, SP_R1,  2, 1, 1), O(SUB, SP_R1,  2, 1, 1), O(ROR,     R0_R2,  2, 1, 2), O(ROL,    R0_R2,  2, 1, 2), O(SHR, R0_R2,  2, 1, 2), O(SHL, R0_R2,  2, 1, 2),
		O(MOV, R1_R0,  2, 1, 2), O(ADD, R1_R0,  2, 1, 2), O(SUB, R1_R0,  2, 1, 2), O(XOR, R1_R0,  2, 1, 2), O(OR,   R1_R0,  2, 1, 2), O(AND,   R1_R0,  2, 1, 2), O(CMP,  R1_R0,  2, 1, 2),                          O(PUSH, R2,    1, 1, 1), O(POP, R2,    1, 1, 1), O(ADD, SP_R2,  2, 1, 1), O(SUB, SP_R2,  2, 1, 1), O(ROR,     R1_R0,  2, 1, 2), O(ROL,    R1_R0,  2, 1, 2), O(SHR, R1_R0,  2, 1, 2), O(SHL, R1_R0,  2, 1, 2),
		O(MOV, R1_R2,  2, 1, 2), O(ADD, R1_R2,  2, 1, 2), O(SUB, R1_R2,  2, 1, 2), O(XOR, R1_R2,  2, 1, 2), O(OR,   R1_R2,  2, 1, 2), O(AND,   R1_R2,  2, 1, 2), O(CMP,  R1_R2,  2, 1, 2), O(JMP,  LABEL, 1, 3, 3), O(PUSH, IMM,   1, 3, 3), O(POP, DISC,  1, 1, 1), O(ADD, SP_IMM, 2, 3, 3), O(SUB, SP_IMM, 2, 3, 3), O(ROR,     R1_R2,  2, 1, 2), O(ROL,    R1_R2,  2, 1, 2), O(SHR, R1_R2,  2, 1, 2), O(SHL, R1_R2,  2, 1, 2),
		O(MOV, R2_R0,  2, 1, 2), O(ADD, R2_R0,  2, 1, 2), O(SUB, R2_R0,  2, 1, 2), O(XOR, R2_R0,  2, 1, 2), O(OR,   R2_R0,  2, 1, 2), O(AND,   R2_R0,  2, 1, 2), O(CMP,  R2_R0,  2, 1, 2), O(CALL, LABEL, 1, 3, 3), O(PUSH, MEM,   1, 3, 3), O(POP, MEM,   1, 3, 3), O(ADD, SP_MEM, 2, 3, 3), O(SUB, SP_MEM, 2, 3, 3), O(ROR,     R2_R0,  2, 1, 2), O(ROL,    R2_R0,  2, 1, 2), O(SHR, R2_R0,  2, 1, 2), O(SHL, R2_R0,  2, 1, 2),
		O(MOV, R2_R1,  2, 1, 2), O(ADD, R2_R1,  2, 1, 2), O(SUB, R2_R1,  2, 1, 2), O(XOR, R2_R1,  2, 1, 2), O(OR,   R2_R1,  2, 1, 2), O(AND,   R2_R1,  2, 1, 2), O(CMP,  R2_R1,  2, 1, 2), O(RET,  NONE,  0, 1, 1), O(NOT,  R0,    1, 1, 1), O(NOT, R1,    1, 1, 1), O(NOT, R2,     1, 1, 1), O(NOT, MEM,    1, 3, 3), O(ROR,     R2_R1,  2, 1, 2), O(ROL,    R2_R1,  2, 1, 2), O(SHR, R2_R1,  2, 1, 2), O(SHL, R2_R1,  2, 1, 2),
		O(MOV, R0_IMM, 2, 3, 3), O(ADD, R0_IMM, 2, 3, 3), O(SUB, R0_IMM, 2, 3, 3), O(XOR, R0_IMM, 2, 3, 3), O(OR,   R0_IMM, 2, 3, 3), O(AND,   R0_IMM, 2, 3, 3), O(CMP,  R0_IMM, 2, 3, 3), O(JL,   LABEL, 1, 3, 3), O(JG,   LABEL, 1, 3, 3), O(JE,  LABEL, 1, 3, 3), O(JO,  LABEL,  1, 3, 3), O(JNO, LABEL,  1, 3, 3), O(ROR,     R0_IMM, 2, 3, 3), O(ROL,    R0_IMM, 2, 3, 3), O(SHR, R0_IMM, 2, 3, 3), O(SHL, R0_IMM, 2, 3, 3),
		O(MOV, R1_IMM, 2, 3, 3), O(ADD, R1_IMM, 2, 3, 3), O(SUB, R1_IMM, 2, 3, 3), O(XOR, R1_IMM, 2, 3, 3), O(OR,   R1_IMM, 2, 3, 3), O(AND,   R1_IMM, 2, 3, 3), O(CMP,  R1_IMM, 2, 3, 3), O(JLE,  LABEL, 1, 3, 3), O(JGE,  LABEL, 1, 3, 3), O(JNE, LABEL, 1, 3, 3), O(JC,  LABEL,  1, 3, 3), O(JNC, LABEL,  1, 3, 3), O(ROR,     R1_IMM, 2, 3, 3), O(ROL,    R1_IMM, 2, 3, 3), O(SHR, R1_IMM, 2, 3, 3), O(SHL, R1_IMM, 2, 3, 3),
		O(MOV, R2_IMM, 2, 3, 3), O(ADD, R2_IMM, 2, 3, 3), O(SUB, R2_IMM, 2, 3, 3), O(XOR, R2_IMM, 2, 3, 3), O(OR,   R2_IMM, 2, 3, 3), O(AND,   R2_IMM, 2, 3, 3), O(CMP,  R2_IMM, 2, 3, 3),                                                                                                                             O(ROR,     R2_IMM, 2, 3, 3), O(ROL,    R2_IMM, 2, 3, 3), O(SHR, R2_IMM, 2, 3, 3), O(SHL, R2_IMM, 2, 3, 3),
		O(MOV, R0_MEM, 2, 3, 4), O(ADD, R0_MEM, 2, 3, 4), O(SUB, R0_MEM, 2, 3, 4), O(XOR, R0_MEM, 2, 3, 4), O(OR,   R0_MEM, 2, 3, 4), O(AND,   R0_MEM, 2, 3, 4), O(CMP,  R0_MEM, 2, 3, 4),                                                                                                                             O(ROR,     R0_MEM, 2, 3, 4), O(ROL,    R0_MEM, 2, 3, 4), O(SHR, R0_MEM, 2, 3, 4), O(SHL, R0_MEM, 2, 3, 4),
		O(MOV, R1_MEM, 2, 3, 4), O(ADD, R1_MEM, 2, 3, 4), O(SUB, R1_MEM, 2, 3, 4), O(XOR, R1_MEM, 2, 3, 4), O(OR,   R1_MEM, 2, 3, 4), O(AND,   R1_MEM, 2, 3, 4), O(CMP,  R1_MEM, 2, 3, 4),                                                                                                                             O(ROR,     R1_MEM, 2, 3, 4), O(ROL,    R1_MEM, 2, 3, 4), O(SHR, R1_MEM, 2, 3, 4), O(SHL, R1_MEM, 2, 3, 4),
		O(MOV, R2_MEM, 2, 3, 4), O(ADD, R2_MEM, 2, 3, 4), O(SUB, R2_MEM, 2, 3, 4), O(XOR, R2_MEM, 2, 3, 4), O(OR,   R2_MEM, 2, 3, 4), O(AND,   R2_MEM, 2, 3, 4), O(CMP,  R2_MEM, 2, 3, 4),                                                                                                                             O(ROR,     R2_MEM, 2, 3, 4), O(ROL,    R2_MEM, 2, 3, 4), O(SHR, R2_MEM, 2, 3, 4), O(SHL, R2_MEM, 2, 3, 4),
		O(MOV, MEM_R0, 2, 3, 4), O(ADD, MEM_R0, 2, 3, 4), O(SUB, MEM_R0, 2, 3, 4), O(XOR, MEM_R0, 2, 3, 4), O(OR,   MEM_R0, 2, 3, 4), O(AND,   MEM_R0, 2, 3, 4), O(CMP,  MEM_R0, 2, 3, 4),                                                                                                                             O(ROR,     MEM_R0, 2, 3, 4), O(ROL,    MEM_R0, 2, 3, 4), O(SHR, MEM_R0, 2, 3, 4), O(SHL, MEM_R0, 2, 3, 4),
		O(MOV, MEM_R1, 2, 3, 4), O(ADD, MEM_R1, 2, 3, 4), O(SUB, MEM_R1, 2, 3, 4), O(XOR, MEM_R1, 2, 3, 4), O(OR,   MEM_R1, 2, 3, 4), O(AND,   MEM_R1, 2, 3, 4), O(CMP,  MEM_R1, 2, 3, 4),                                                                                                                             O(ROR,     MEM_R1, 2, 3, 4), O(ROL,    MEM_R1, 2, 3, 4), O(SHR, MEM_R1, 2, 3, 4), O(SHL, MEM_R1, 2, 3, 4),
		O(MOV, MEM_R2, 2, 3, 4), O(ADD, MEM_R2, 2, 3, 4), O(SUB, MEM_R2, 2, 3, 4), O(XOR, MEM_R2, 2, 3, 4), O(OR,   MEM_R2, 2, 3, 4), O(AND,   MEM_R2, 2, 3, 4), O(CMP,  MEM_R2, 2, 3, 4),                          O(INC,  R0,    1, 1, 1), O(INC,  R1,   1, 1, 1), O(INC, R2,     1, 1, 1), O(INC, MEM,    1, 3, 3), O(ROR,     MEM_R2, 2, 3, 4), O(ROL,    MEM_R2, 2, 3, 4), O(SHR, MEM_R2, 2, 3, 4), O(SHL, MEM_R2, 2, 3, 4),
		O(XOR, R0_R0,  2, 1, 2), O(XOR, R1_R1,  2, 1, 2), O(XOR, R2_R2,  2, 1, 2), O(SWI, NONE,   0, 1, 1), O(IRET, NONE,   0, 1, 1), O(RESET, NONE,   0, 1, 1), O(PUSH, FLAGS,  1, 1, 1), O(POP,  FLAGS, 1, 1, 1), O(DEC,  R0,    1, 1, 1), O(DEC,  R1,   1, 1, 1), O(DEC, R2,     1, 1, 1), O(DEC, MEM,    1, 3, 3), O(PUSHALL, NONE,   0, 1, 1), O(POPALL, NONE,   0, 1, 1), O(EI,  NONE,   0, 1, 1), O(DI,  NONE,   0, 1, 1),

		/*{ MOV_R0_R1,  { 2_u8, 1_u8, 2_u8 } }, { ADD_R0_R1,  { 2_u8, 1_u8, 2_u8 } }, { SUB_R0_R1,  { 2_u8, 1_u8, 2_u8 } },
		{ MOV_R0_R2,  { 2_u8, 1_u8, 2_u8 } }, { ADD_R0_R2,  { 2_u8, 1_u8, 2_u8 } },	{ SUB_R0_R2,  { 2_u8, 1_u8, 2_u8 } },
		{ MOV_R1_R0,  { 2_u8, 1_u8, 2_u8 } }, { ADD_R1_R0,  { 2_u8, 1_u8, 2_u8 } },	{ SUB_R1_R0,  { 2_u8, 1_u8, 2_u8 } },
		{ MOV_R1_R2,  { 2_u8, 1_u8, 2_u8 } }, { ADD_R1_R2,  { 2_u8, 1_u8, 2_u8 } },	{ SUB_R1_R2,  { 2_u8, 1_u8, 2_u8 } },
		{ MOV_R2_R0,  { 2_u8, 1_u8, 2_u8 } }, { ADD_R2_R0,  { 2_u8, 1_u8, 2_u8 } },	{ SUB_R2_R0,  { 2_u8, 1_u8, 2_u8 } },
		{ MOV_R2_R1,  { 2_u8, 1_u8, 2_u8 } }, { ADD_R2_R1,  { 2_u8, 1_u8, 2_u8 } },	{ SUB_R2_R1,  { 2_u8, 1_u8, 2_u8 } },
		{ MOV_R0_IMM, { 2_u8, 3_u8, 3_u8 } }, { ADD_R0_IMM, { 2_u8, 3_u8, 3_u8 } },	{ SUB_R0_IMM, { 2_u8, 3_u8, 3_u8 } },
		{ MOV_R1_IMM, { 2_u8, 3_u8, 3_u8 } }, { ADD_R1_IMM, { 2_u8, 3_u8, 3_u8 } },	{ SUB_R1_IMM, { 2_u8, 3_u8, 3_u8 } },
		{ MOV_R2_IMM, { 2_u8, 3_u8, 3_u8 } }, { ADD_R2_IMM, { 2_u8, 3_u8, 3_u8 } },	{ SUB_R2_IMM, { 2_u8, 3_u8, 3_u8 } },
		{ MOV_R0_MEM, { 2_u8, 3_u8, 4_u8 } }, { ADD_R0_MEM, { 2_u8, 3_u8, 4_u8 } },	{ SUB_R0_MEM, { 2_u8, 3_u8, 4_u8 } },
		{ MOV_R1_MEM, { 2_u8, 3_u8, 4_u8 } }, { ADD_R1_MEM, { 2_u8, 3_u8, 4_u8 } },	{ SUB_R1_MEM, { 2_u8, 3_u8, 4_u8 } },
		{ MOV_R2_MEM, { 2_u8, 3_u8, 4_u8 } }, { ADD_R2_MEM, { 2_u8, 3_u8, 4_u8 } },	{ SUB_R2_MEM, { 2_u8, 3_u8, 4_u8 } },
		{ MOV_MEM_R0, { 2_u8, 3_u8, 4_u8 } }, { ADD_MEM_R0, { 2_u8, 3_u8, 4_u8 } },	{ SUB_MEM_R0, { 2_u8, 3_u8, 4_u8 } },
		{ MOV_MEM_R1, { 2_u8, 3_u8, 4_u8 } }, { ADD_MEM_R1, { 2_u8, 3_u8, 4_u8 } },	{ SUB_MEM_R1, { 2_u8, 3_u8, 4_u8 } },
		{ MOV_MEM_R2, { 2_u8, 3_u8, 4_u8 } }, { ADD_MEM_R2, { 2_u8, 3_u8, 4_u8 } },	{ SUB_MEM_R2, { 2_u8, 3_u8, 4_u8 } },
		{ XOR_R0_R0,  { 2_u8, 1_u8, 2_u8 } }, { XOR_R1_R1,  { 2_u8, 1_u8, 2_u8 } }, { XOR_R2_R2,  { 2_u8, 1_u8, 2_u8 } },*/
	};
#undef O
}
#pragma endregion

#pragma region MemoryOperations
//read 8 bits of information from memory
auto& cpu::Processor::read8(void) noexcept
{
	DB = static_cast<std::uint16_t>(MEM[AB]);
	clock();
	return MEM[AB];
}

//write 8 bits of information to memory
auto cpu::Processor::write8(void) noexcept
{
	MEM[AB] = static_cast<std::uint8_t>(DB);
	clock();
}

//read 16 bits of information from memory
auto& cpu::Processor::read16(void) noexcept
{
	//read in the upper and lower halves
	auto lower = read8();
	++AB;
	auto upper = read8();

	//compose them into a single value and return
	DB = ::compose(lower, upper);
	return DB;
}

//write 16 bits of information to memory
auto cpu::Processor::write16(void) noexcept
{
	//get the upper and lower halves of the value
	auto [upper, lower] = ::decompose(DB);

	//write out each byte
	DB = upper;
	write8();

	++AB;

	DB = lower;
	write8();
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
	clock();
}

//pop 8 bits of information from the stack
auto cpu::Processor::pop8(void) noexcept
{
	//deallocate the stack slot
	++SP;

	//read the data
	AB = SP - 1_u16;
	DB = MEM[AB];
	clock();
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
#pragma endregion

#pragma region InstructionDefinition
//general instruction implementation

//x: applied operator
//y: 16bit cycle count
//z: 8bit  cycle count
#define GNRL_INSN_IMPL(x, y, z)                                            \
	if (!EF.SIZE_OVERRIDE) [[likely]]                                      \
	{                                                                      \
		dst.ALL x src.ALL;                                                 \
		for (auto i = 0_uz; i < y; ++i) clock();                           \
	}                                                                      \
	else [[unlikely]]                                                      \
	{                                                                      \
		if (EF.IS_UPPER_DEST and EF.IS_UPPER_SRCE) [[unlikely]]            \
		{                                                                  \
			if (EF.ZERO_EXTENDED) [[unlikely]]                             \
			{                                                              \
				dst.ALL = 0_u16;                                           \
				dst.HI x src.HI;                                           \
			}                                                              \
			else [[likely]]                                                \
			{                                                              \
				dst.HI x src.HI;                                           \
			}                                                              \
		}                                                                  \
		else if (EF.IS_UPPER_DEST and not EF.IS_UPPER_SRCE) [[unlikely]]   \
		{                                                                  \
			if (EF.ZERO_EXTENDED) [[unlikely]]                             \
			{                                                              \
				dst.ALL = 0_u16;                                           \
				dst.HI x src.LO;                                           \
			}                                                              \
			else [[likely]]                                                \
			{                                                              \
				dst.HI x src.LO;                                           \
			}                                                              \
		}                                                                  \
		else if (not EF.IS_UPPER_DEST and EF.IS_UPPER_SRCE) [[unlikely]]   \
		{                                                                  \
			if (EF.ZERO_EXTENDED) [[unlikely]]                             \
			{                                                              \
				dst.ALL = 0_u16;                                           \
				dst.LO x src.HI;                                           \
			}                                                              \
			else [[likely]]                                                \
			{                                                              \
				dst.LO x src.HI;                                           \
			}                                                              \
		}                                                                  \
		else if (not EF.IS_UPPER_DEST and not EF.IS_UPPER_SRCE) [[likely]] \
		{                                                                  \
			if (EF.ZERO_EXTENDED) [[unlikely]]                             \
			{                                                              \
				dst.ALL = 0_u16;                                           \
				dst.LO x src.LO;                                           \
			}                                                              \
			else [[likely]]                                                \
			{                                                              \
				dst.LO x src.LO;                                           \
			}                                                              \
		}                                                                  \
		for (auto i = 0_uz; i < z; ++i) clock();                           \
	}                                                                      \

//x: applied operator
//y: 16bit cycle count
//z: 8bit  cycle count
//u: 16bit insn byte count
//v: 8bit  insn byte count

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
#pragma endregion

//runs the instruction pointed to by the instruction pointer
auto cpu::Processor::run(std::uint8_t opcode) noexcept
{
	//determine what to do based on that byte
	switch (opcode)
	{
#define GNRL_REG_REG_INSN(x, y, z, w)   \
		case ::Opcode::w##_##x##_##y:   \
			return do_##z##_insn(x, y); \

#define FOR_EACH_REG(x, y)              \
		GNRL_REG_REG_INSN(R0, R1, x, y) \
		GNRL_REG_REG_INSN(R0, R2, x, y) \
		GNRL_REG_REG_INSN(R1, R0, x, y) \
		GNRL_REG_REG_INSN(R1, R2, x, y) \
		GNRL_REG_REG_INSN(R2, R0, x, y) \
		GNRL_REG_REG_INSN(R2, R1, x, y) \

		FOR_EACH_REG(move, MOV)
		FOR_EACH_REG(add, ADD)
		FOR_EACH_REG(sub, SUB)
		FOR_EACH_REG(xor, XOR)
		FOR_EACH_REG(or, OR)
		FOR_EACH_REG(and, AND)
#undef FOR_EACH_REG
#undef GNRL_REG_REG_INSN

#ifndef SET_INTERRUPT
#define SET_INTERRUPT(x, y)                      \
		case ::Opcode::x:                        \
		{                                        \
			SF.IF = y;                           \
			clock();                             \
			return 1_u16;                        \
		}                                        \

	   SET_INTERRUPT(EI, true)
	   SET_INTERRUPT(DI, false)
#undef SET_INTERRUPT
#else
static_assert(false, "Redefinition of SET_INTERRUPT");
#endif

		case ::Opcode::POP_DISCARD:
		{
			if (!EF.SIZE_OVERRIDE) [[likely]]
			{
				static_cast<void>(pop16());

				//1 byte
				return 1_u16;
			}

			else [[unlikely]]
			{
				static_cast<void>(pop8());

				//2 bytes
				return 2_u16;
			}
		}

		case ::Opcode::PUSH_FLAGS:
		{
			//push the status flags register
			push16(SF.ALL);

			//1 byte
			return 1_u16;
		}

		case ::Opcode::POP_FLAGS:
		{
			//pop the status flags register
			SF.ALL = pop16();

			//1 byte
			return 1_u16;
		}

		case ::Opcode::PUSHALL:
		{
			//save the general purpose registers
			push16(R0.ALL);
			push16(R1.ALL);
			push16(R2.ALL);

			//save the status flags register
			push16(SF.ALL);

			//1 byte
			return 1_u16;
		}

		case ::Opcode::POPALL:
		{
			//pop the status flags register
			SF.ALL = pop16();

			//pop the general purpose registers
			R2.ALL = pop16();
			R1.ALL = pop16();
			R0.ALL = pop16();

			//1 byte
			return 1_u16;
		}

		case ::Opcode::SWINT:
		{
			//save the current instruction pointer
			push16(IP);

			//jump to the software interrupt vector and read it
			AB = 0xFFF3;

			//jump to the specified interrupt handler
			IP = read16();

			//1 byte
			return 1_u16;
		}

		case ::Opcode::RESET:
		{
			//do the reset operation
			reset();

			//1 byte
			return 1_u16;
		}
	}
}

//resets the cpu and begins instruction sequence execution
void cpu::Processor::reset(void) noexcept
{
	clock();

	//put known good values in each register
	R0.ALL = 0x0000,
	R1.ALL = 0x0000,
	R2.ALL = 0x0000,
	SP = 0xFFF0;

	clock();

	//put the reset vector out on the address bus
	AB = 0xFFF1;

	clock();

	//jump to the address pointed to by the reset vector
	IP = read16();

	clock();
}

//case 0x87: EF = { 0, 0, 0, 0 }; ++AB; break;
//case 0x88: EF = { 0, 0, 1, 0 }; ++AB; break;
//case 0x97: EF = { 1, 0, 0, 0 }; ++AB; break;
//case 0x98: EF = { 1, 0, 1, 0 }; ++AB; break;
//case 0xA7: EF = { 0, 1, 0, 0 }; ++AB; break;
//case 0xA8: EF = { 0, 1, 1, 0 }; ++AB; break;
//case 0xB7: EF = { 1, 1, 0, 0 }; ++AB; break;
//case 0xB8: EF = { 1, 1, 1, 0 }; ++AB; break;

//runs an entire instruction sequence
void cpu::Processor::execute(void) noexcept
{
	//save a local copy of the instruction pointer
	auto ip_copy = IP;

	//defines each state of the execution state machine
	enum class State
	{
		Complete,
		Prefix,
		Instruction,
		Operand1,
		Operand2,
	} state = State::Prefix;

	//map: { opcode } -> tuple{ operand count, byte count 16size, byte count 8size }
	std::uint8_t operand_count = 0_u8,
				 byte_count_16 = 0_u8,
				 byte_count_8  = 0_u8;

	//stores the decoded opcode
	std::uint8_t opcode = 0_u8;

	//stores the decoded operand 1 and 2
	cpu::Register *operand1 = nullptr,
				  *operand2 = nullptr;

	//loop until a break condition is hit
	while (true)
	{
		//configure the address bus for reading data
		AB = ip_copy;

		//read the first byte of the instruction
		auto& fetched = read8();

		//state machine code
		switch (state)
		{
		case State::Prefix:
			switch (fetched)
			{
				//aggregate initialization for bitfields order:
				//IS_UPPER_SRCE, IS_UPPER_DEST, ZERO_EXTENDED, SIZE_OVERRIDE

				case 0x89: EF = { .IS_UPPER_SRCE{ 0 }, .IS_UPPER_DEST{ 0 }, .ZERO_EXTENDED{ 0 }, .SIZE_OVERRIDE{ 1 } }; ++AB; fetched = read8(); break;
				case 0x8A: EF = { .IS_UPPER_SRCE{ 0 }, .IS_UPPER_DEST{ 0 }, .ZERO_EXTENDED{ 1 }, .SIZE_OVERRIDE{ 1 } }; ++AB; fetched = read8(); break;

				case 0x99: EF = { .IS_UPPER_SRCE{ 1 }, .IS_UPPER_DEST{ 0 }, .ZERO_EXTENDED{ 0 }, .SIZE_OVERRIDE{ 1 } }; ++AB; fetched = read8(); break;
				case 0x9A: EF = { .IS_UPPER_SRCE{ 1 }, .IS_UPPER_DEST{ 0 }, .ZERO_EXTENDED{ 1 }, .SIZE_OVERRIDE{ 1 } }; ++AB; fetched = read8(); break;

				case 0xA9: EF = { .IS_UPPER_SRCE{ 0 }, .IS_UPPER_DEST{ 1 }, .ZERO_EXTENDED{ 0 }, .SIZE_OVERRIDE{ 1 } }; ++AB; fetched = read8(); break;
				case 0xAA: EF = { .IS_UPPER_SRCE{ 0 }, .IS_UPPER_DEST{ 1 }, .ZERO_EXTENDED{ 1 }, .SIZE_OVERRIDE{ 1 } }; ++AB; fetched = read8(); break;

				case 0xB9: EF = { .IS_UPPER_SRCE{ 1 }, .IS_UPPER_DEST{ 1 }, .ZERO_EXTENDED{ 0 }, .SIZE_OVERRIDE{ 1 } }; ++AB; fetched = read8(); break;
				case 0xBA: EF = { .IS_UPPER_SRCE{ 1 }, .IS_UPPER_DEST{ 1 }, .ZERO_EXTENDED{ 1 }, .SIZE_OVERRIDE{ 1 } }; ++AB; fetched = read8(); break;

				default:   EF = { .IS_UPPER_SRCE{ 0 }, .IS_UPPER_DEST{ 0 }, .ZERO_EXTENDED{ 0 }, .SIZE_OVERRIDE{ 0 } };                          break;
			}
			//switch to a instruction reading context
			state = State::Instruction;
			[[fallthrough]];

		case State::Instruction:
			//decode the current instruction and extract information about it
			std::tie(operand_count, byte_count_16, byte_count_8) = opcodes[fetched];


			//update the state machine
			switch (operand_count)
			{
				case 0: state = State::Complete;
				case 1: state = State::Operand1;
				case 2: state = State::Operand1;
			}
			break;

		case State::Operand1:
			switch (operand_count)
			{
				case 1: state = State::Complete;
				case 2: state = State::Operand2;
			}
			break;

		case State::Operand2:
			
			state = State::Complete;
			break;

		case State::Complete:
			//execute the instruction--execution prefix flags are already configured correctly
			
			


			break;
		}
	}




	while (true) //replace with something correct later
	{
		//copy the instruction pointer locally
		auto ip_copy = IP;

		while (true) //replace with something correct later
		{
			//determine if the fetched byte was a prefix flag byte
			switch (fetched)
			{
				//aggregate initialization for bitfields order:
				//IS_UPPER_SRCE, IS_UPPER_DEST, ZERO_EXTENDED, SIZE_OVERRIDE

				//case 0x87: EF = { 0, 0, 0, 0 }; ++AB; break;
				//case 0x88: EF = { 0, 0, 1, 0 }; ++AB; break;
				case 0x89: EF = { 0, 0, 0, 1 }; ++AB; break;
				case 0x8A: EF = { 0, 0, 1, 1 }; ++AB; break;
				//case 0x97: EF = { 1, 0, 0, 0 }; ++AB; break;
				//case 0x98: EF = { 1, 0, 1, 0 }; ++AB; break;
				case 0x99: EF = { 1, 0, 0, 1 }; ++AB; break;
				case 0x9A: EF = { 1, 0, 1, 1 }; ++AB; break;
				//case 0xA7: EF = { 0, 1, 0, 0 }; ++AB; break;
				//case 0xA8: EF = { 0, 1, 1, 0 }; ++AB; break;
				case 0xA9: EF = { 0, 1, 0, 1 }; ++AB; break;
				case 0xAA: EF = { 0, 1, 1, 1 }; ++AB; break;
				//case 0xB7: EF = { 1, 1, 0, 0 }; ++AB; break;
				//case 0xB8: EF = { 1, 1, 1, 0 }; ++AB; break;
				case 0xB9: EF = { 1, 1, 0, 1 }; ++AB; break;
				case 0xBA: EF = { 1, 1, 1, 1 }; ++AB; break;

				default:   EF = { 0, 0, 0, 0 }; break;
			}

			//execute the instruction with the specified flags
			auto inc = run(read8());

			//increment the instruction pointer based on the instruction length
			IP += inc;
		}
	}

	

	
}

//run for each clock cycle of the system
void cpu::Processor::clock(void) noexcept
{
	//check whether status information should be printed out
	if (debugging)
	{
		//invoke the printing function
		::debug(this);
	}

	//retrieve the requested execution speed
	auto speed = ::compose(MEM[0xFFFC], MEM[0xFFFB]);

	/*Verify that we don't do a division by zero, but as a good
	side effect, we can zero out the speed register and get different
	behavior. In our case, we use this to indiciate single step mode.*/
	if (speed == 0) [[unlikely]]
	{
		//something about single step mode
		//enter key hack for now is ok
		std::cin.get();
	}

	//otherwise, wait
	else [[likely]]
	{
		/*The speed register indicates approximate execution frequency,
		so to wait for the correct amount of time, we must take the inverse*/
		std::this_thread::sleep_for(std::chrono::duration<double, std::milli>
		{
			(1.0 / static_cast<double>(static_cast<std::uint64_t>(speed)))
		});
	}
}