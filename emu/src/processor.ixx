export module processor;

export import common;

export namespace cpu
{
	struct Processor
	{
	public:
		//flags register
		std::bitset<cpu::constants::reg_size> FLAGS;
		
		//instruction pointer register
		std::uint16_t IP;

		//stack pointer register
		std::uint16_t SP;

		//general purpose registers
		std::uint16_t R0, R1, R2, R3;

		//data and address 'buses'
		std::uint16_t DB, AB;

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