#ifndef emu_processor_hpp
#define emu_processor_hpp

//disable the nonstandard extension used: anonymous struct
#pragma warning(push)
#pragma warning(disable: 4201)

#include <cstdint>
#include <vector>
#include <limits>
#include <array>

#include "literals.hpp"

/*
  Warning: this file contains several instances of known current standard C++ undefined and/or unsupported behavior, and likely undefined
  and/or unsupported behavior for future standard C++ revisions. In one embodiment these include but are not limited to untagged and
  globally uninstantiated structures embodying one or more unnamed scoped union members i.e. within a non-anonymous union context.
  In one embodiment these include but are not limited to the expectation that inactive union member accesses embody one or more
  bytes within the memory allocated for the union itself, and that these bytes embody the variable aligned non-padded nonstatic
  union members themselves and/or strict or relaxed aliases to them, and that these bytes can be determined statically to be of non-trap
  representation. In one embodiment this behavior specifies non-structure-heading structure padding for one or more nonstatic bitfield
  structure members, and asserts that non-byte aligned nonstatic bitfield structure members will be padded by the user such that the
  compiler itself can sequester control over one or more bytes of a complete, but uninstantiated structure and/or the bytes that would
  immediately follow it were they to be mapped to physical and/or logical processor address space to conform to user specified structure
  memory boundary alignment. In another embodiment, this behavior asserts that non-byte aligned nonstatic bitself structure members will
  be aligned to byte and/or user specified structure memory boundaries. In one embodiment these include the assurance that the compiler
  shall not place structure padding between nonstatic bitfield structure members, unless the user specifies as such by creating one or more
  zero sized, unnamed, and complete typed nonstatic bitfield structure members between other instances of nonzero sized, named, and complete
  typed nonstatic bitfield structure members. In the last embodiment, these include that the compiler can sequester control over the ordering
  of nonstatic bitfield structure members, only if doing so would result in a net loss in structure padding, and/or a noticeable net gain in
  real-world execution performance. Implied in this embodiment is the assertion that native processor word sized byte alignment specifications are
  preferrable to non-native processor word sized byte alignment specifications for reading and/or writing to memory, including but not limited to
  single reference accesses and native and/or non native structured or primitive typed memory-possible array strides i.e. unconstrained from cache
  memory usage and/or the use of zero or more native processor general-purpose operations registers i.e. REX, MMX, XMMX, YMMX, ZMMX, and/or 
  STX floating point unit stack register registers on the x86_64 family of processors. While initial testing on MSVC v142, clang++ v11.0 and
  GCC v12.0 would indicate that each embodiment of the compiler specific vendor extension requirements list are satisfied with no constraints
  on any of the listed platforms, apart from that embodiments themselves not represent any other embodiable embodiments, there is no implied
  warrantee nor guarantee on the execution performance and/or conformance of the software in relation to what might be expected by an end user.
*/

namespace cpu
{
#if defined(_MSC_VER) and defined(_MSC_EXTENSIONS) and defined(_M_X64)
	union Register
	{
	public:
		std::uint16_t ALL;   //both bytes

		struct
		{
			std::uint8_t LO, //low  byte
						 HI; //high byte
		};

	public:
		constexpr Register(std::uint16_t ALL) noexcept : ALL(ALL) {}
		constexpr Register(std::uint8_t LO, std::uint8_t HI) noexcept : LO(LO), HI(HI) {}
		constexpr ~Register() noexcept {}
	};
#else
static_assert(false, "This project may or may not be compatible with your platform. Inspect the code snippet above and verify"
					 " that your compiler extensions support safe reading of inactive union members and anonymous structs");
#endif

	struct Processor
	{
	public:
		//flags register
		union
		{
			struct
			{
				//this only struct-ized so the fields don't collapse in the surrounding union
				std::uint16_t ZF : 1,  //zero flag
							  CF : 1,  //carry flag
							  OF : 1,  //overflow flag
							  IF : 1,  //interrupt flag
							     : 12; //padding
			};

			std::uint16_t ALL;
		} SF;

		//verify that we can pun this into an integer when needed
		static_assert(sizeof(SF) == sizeof(std::uint16_t));

		//instruction pointer register
		std::uint16_t IP = 0xFFF1_u16;

		//stack pointer register
		std::uint16_t SP = 0xFFF0_u16;

		//general purpose registers
		cpu::Register R0 = { 0x0000_u16 },
					  R1 = { 0x0000_u16 },
					  R2 = { 0x0000_u16 };

		//data and address 'buses'
		std::uint16_t DB = 0x0000_u16,
					  AB = 0x0000_u16;

		//temporary execution flags
		struct
		{
			//this only struct-ized so the fields don't collapse in the surrounding union
			std::uint8_t IS_UPPER_SRCE : 1, //source is upper byte
						 IS_UPPER_DEST : 1, //destination is upper byte
						 ZERO_EXTENDED : 1, //should zero extend byte operations
						 SIZE_OVERRIDE : 1, //operands are both 8 bits
						               : 4; //padding
		} EF;

		//memory that corresponds to the entire processor address space
		std::array<std::uint8_t, std::numeric_limits<std::uint16_t>::max()> MEM{};

		/*This flag is set if and only if the current program was invoked by the debugger*/
		/*      executable, and requires the output of processor status information      */
		bool debugging = false;

	public:
		//custom c'tor
		constexpr Processor(const std::vector<std::uint8_t>& bin) noexcept
		{
			//copy binary contents to memory starting at 8000h
			std::copy(bin.cbegin(), bin.cend(), (MEM.begin() + 0x8000));
		}

		constexpr ~Processor(void) noexcept
		{
		}

	private:
#pragma region MemoryOperations
		//read 8 bits of information from memory
		auto& read8(void) noexcept;
		
		//write 8 bits of information to memory
		auto write8(void) noexcept;

		//read 16 bits of information from memory
		auto& read16(void) noexcept;

		//write 16 bits of information to memory
		auto write16(void) noexcept;

		//push 8 bits of information to the stack
		auto push8(std::uint8_t) noexcept;

		//pop 8 bits of information from the stack
		auto pop8(void) noexcept;

		//push 16 bits of information to the stack
		auto push16(std::uint16_t) noexcept;

		//pop 16 bits of information from the stack
		auto pop16(void) noexcept;
#pragma endregion

#pragma region InstructionDeclarations
		//move instruction implementaton
		std::uint16_t do_move_insn(cpu::Register&, const cpu::Register&) noexcept;

		//add instruction implementaton
		std::uint16_t do_add_insn(cpu::Register&, const cpu::Register&) noexcept;

		//sub instruction implementation
		std::uint16_t do_sub_insn(cpu::Register&, const cpu::Register&) noexcept;

		//xor instruction implementation
		std::uint16_t do_xor_insn(cpu::Register&, const cpu::Register&) noexcept;

		//or instruction implementation
		std::uint16_t do_or_insn(cpu::Register&, const cpu::Register&) noexcept;

		//and instruction implementation
		std::uint16_t do_and_insn(cpu::Register&, const cpu::Register&) noexcept;
#pragma endregion

	public:
#pragma region ProcessorOperations
		//runs the instruction pointed to by the instruction pointer
		std::uint16_t run(std::uint8_t) noexcept;

		//begins instruction sequence execution
		void execute(void) noexcept;

		//resets the processor state
		void reset(void) noexcept;

		//runs for each clock cycle of the system
		void clock(void) noexcept;
#pragma endregion
	};
}

#pragma warning(pop)
#endif