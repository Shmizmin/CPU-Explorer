import assemble;

//internally linked functions
namespace
{
	//specialized std::byte integer literal
	inline constexpr std::byte operator"" _byte(std::size_t n) noexcept
	{
		return std::byte{ static_cast<std::underlying_type_t<std::byte>>(n) };
	}

	//splits a string into a vector of strings over a delimiter character or string
	//compress flag suppresses duplicate empty strings when multiple delimiters are stacked
	std::vector<std::string> split(std::string text, std::string delim, bool compress)
	{
		std::vector<std::string> vec;
		std::size_t pos = 0_uz, prevPos = 0_uz;
		while (true)
		{
			pos = text.find_first_of(delim, prevPos);

			while (compress)
			{
				if (prevPos == pos)
					prevPos++;
				else
					break;

				pos = text.find_first_of(delim, prevPos);
			}

			if (pos == std::string::npos)
			{
				if (prevPos != text.size())
					vec.emplace_back(text.substr(prevPos));
				return vec;
			}

			vec.emplace_back(text.substr(prevPos, pos - prevPos));
			prevPos = pos + 1_uz;
		}
	}

	//each of the possible configurations of operands in our assembly langauge
	enum class Operand
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
		DISCARD,
		LABEL,
		NONE,
	};

	//extract operand from a line into a 3 size array of string
	auto find_operands(const std::string& line) noexcept
	{
		//https://regex101.com/r/JVIYVc/1
		std::regex insn(R"(^(?:([a-z]*) ?((?:[a-z]\w*)|(?:r[0-9])|(?:(?:#|%)\$?\w+))?(?:, ?(r[0-9]|(?:(?:#|%)\$?\w+)))?(?: )*(?:;(?:.*))?)$)");

		//will store any regex matches we make later on
		std::smatch insn_matches;

		//a line can only be an instruction or a label
		if (!std::regex_match(line, insn_matches, insn))
		{
			//if it isn't throw an error
			std::cerr << "Assembly Syntax Error";
			std::exit(12);
		}

		//assembly is likely valid so return the matches we made
		else return std::array{ insn_matches[1], insn_matches[2], insn_matches.size() == 4 ? insn_matches[3] : std::ssub_match{} };
	}

	//extract numeric operands
	template<std::size_t index>
	std::uint16_t get_numeric_operand(const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		//untie the operands array
		const auto& [instruciton, op1, op2] = operands;
		
		//if op1 should be decoded
		if constexpr (index == 1_uz)
		{
			//determine how the number is encoded in source
			switch (op1.str()[1])
			{
			case '$':
				//number was encoded in hex
				return static_cast<std::uint16_t>(std::stoul(op1.str().substr(2_uz), 0, 16));

			case '@':
				//number was encoded in binary
				return static_cast<std::uint16_t>(std::stoul(op1.str().substr(2_uz), 0, 2));

			default:
				//number was encoded in decimal
				return static_cast<std::uint16_t>(std::stoul(op1.str().substr(1_uz), 0, 10));
			}
		}

		//if op2 should be decoded
		else if constexpr (index == 2_uz)
		{
			//determine how the number is encoded in source
			switch (op2.str()[1])
			{
			case '$':
				//number was encoded in hex
				return static_cast<std::uint16_t>(std::stoul(op2.str().substr(2_uz), 0, 16));

			case '@':
				//number was encoded in binary
				return static_cast<std::uint16_t>(std::stoul(op2.str().substr(2_uz), 0, 2));

			default:
				//number was encoded in decimal
				return static_cast<std::uint16_t>(std::stoul(op2.str().substr(1_uz), 0, 10));
			}
		}

		//any other index is invalid
		else
		{
			//throw an error during compilation
			static_assert(false, "get_numeric_index requires an index of 1 or 2");
			return 0_u16;
		}

	}

	auto extract_bits(auto& code, auto whole) noexcept
	{
		//extract the 8-bit upper and lower halves
		auto upper = static_cast<std::uint8_t>((whole & 0xFF00) >> 8);
		auto lower = static_cast<std::uint8_t>((whole & 0x00FF) >> 0);

		//encode them directly in the binary
		code.emplace_back(std::byte{ lower });
		code.emplace_back(std::byte{ upper });
	}
	
	template<std::size_t index>
	auto embed_numeric(auto& code, const auto& operands) noexcept
	{
		//decode the number from source
		auto whole = get_numeric_operand<index>(operands);

		//encode it in the binary
		extract_bits(code, whole);
	}
	
	//extract operand encoding type
	auto get_encoding_type(const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		//untie the operands array
		const auto& [instruction, op1, op2] = operands;

		//'state machine' variable for what encoding is detected
		auto encoding = ::Operand::NONE;

		//there must be at least a single operand for encoding to apply
		if (op1.matched)
		{
			switch (op1.str()[0])
			{
			case 'r':
				//register could be unary
				if (!op2.matched)
				{
					//if it is, determine which register is operated on
					switch (op1.str()[1])
					{
						case '0': encoding = ::Operand::R0; break;
						case '1': encoding = ::Operand::R1; break;
						case '2': encoding = ::Operand::R2; break;
					}
				}

				//register is binary
				else
				{
					//determine what the second operand contains
					switch (op2.str()[0])
					{
						case 'r':
						{
							//second operand is another register, therefore the encoding must be case-searched
							//this is kind of a hack-y solution, but it's simple, and fairly fast to search and use
							std::map<std::pair<std::string, std::string>, ::Operand> ops
							{
								{ { "r0", "r0" }, ::Operand::R0_R0 },
								{ { "r0", "r1" }, ::Operand::R0_R1 },
								{ { "r0", "r2" }, ::Operand::R0_R2 },

								{ { "r1", "r0" }, ::Operand::R1_R0 },
								{ { "r1", "r1" }, ::Operand::R1_R2 },
								{ { "r1", "r2" }, ::Operand::R1_R2 },

								{ { "r2", "r0" }, ::Operand::R2_R0 },
								{ { "r2", "r1" }, ::Operand::R2_R2 },
								{ { "r2", "r2" }, ::Operand::R2_R2 }
							};

							encoding = ops.at({ op1, op2 });
							break;
						}

						case '#':
							switch (op1.str()[1])
							{
								case '0': encoding = ::Operand::R0_IMM; break;
								case '1': encoding = ::Operand::R1_IMM; break;
								case '2': encoding = ::Operand::R2_IMM; break;
							}
							break;

						case '%':
							switch (op1.str()[1])
							{
								case '0': encoding = ::Operand::R0_MEM; break;
								case '1': encoding = ::Operand::R1_MEM; break;
								case '2': encoding = ::Operand::R2_MEM; break;
							}
							break;
					}
				}
				break;

			case '#':
				if (!op2.matched)
				{
					encoding = ::Operand::IMM;
				}
				break;

			case '%':
				if (!op2.matched)
				{
					encoding = ::Operand::MEM;
				}

				else if (op2.str()[0] == 'r')
				{
					switch (op2.str()[1])
					{
						case '0': encoding = ::Operand::MEM_R0; break;
						case '1': encoding = ::Operand::MEM_R1; break;
						case '2': encoding = ::Operand::MEM_R2; break;
					}
				}
				break;

			case 'd':
				encoding = ::Operand::DISCARD;
				break;

			case 'i':
				encoding = ::Operand::IP;
				break;

			case 'f':
				encoding = ::Operand::FLAGS;
				break;

			case 'S':
				if (!op2.matched)
				{
					encoding = ::Operand::SP;
				}

				else
				{
					switch (op2.str()[0])
					{
					case 'r':
						switch (op2.str()[1])
						{
							case '0': encoding = ::Operand::SP_R0; break;
							case '1': encoding = ::Operand::SP_R1; break;
							case '2': encoding = ::Operand::SP_R2; break;
						}
						break;

					case '#':
						encoding = ::Operand::SP_IMM;
						break;

					case '%':
						encoding = ::Operand::SP_MEM;
						break;
					}
				}
				break;
			}
		}

		else
		{
			encoding = ::Operand::NONE;
		}

		return encoding;
	}

#pragma region INSNS

#ifndef DEFAULT_CASE
#define DEFAULT_CASE(x) default:                                          \
	std::cerr << "Incorrect argument type passed to instruction " << #x;  \
	std::exit(13);                                                        \
	break;

	void do_decrement(std::vector<std::byte>& code, ::Operand type, const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		switch (type)
		{
			case ::Operand::R0:  code.emplace_back(0xF8_byte); break;
			case ::Operand::R1:  code.emplace_back(0xF9_byte); break;
			case ::Operand::R2:  code.emplace_back(0xFA_byte); break;
			case ::Operand::MEM: code.emplace_back(0xFB_byte); embed_numeric<1_uz>(code, operands); break;
			DEFAULT_CASE(dec)
		}
	}

	void do_increment(std::vector<std::byte>& code, ::Operand type, const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		switch (type)
		{
			case ::Operand::R0:  code.emplace_back(0xE8_byte); break;
			case ::Operand::R1:  code.emplace_back(0xE9_byte); break;
			case ::Operand::R2:  code.emplace_back(0xEA_byte); break;
			case ::Operand::MEM: code.emplace_back(0xEB_byte); embed_numeric<1_uz>(code, operands); break;
			DEFAULT_CASE(inc)
		}
	}

	void do_cmp(std::vector<std::byte>& code, ::Operand type, const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		switch (type)
		{
			case ::Operand::R0_R1:  code.emplace_back(0x06_byte); break;
			case ::Operand::R0_R2:  code.emplace_back(0x16_byte); break;

			case ::Operand::R1_R0:  code.emplace_back(0x26_byte); break;
			case ::Operand::R1_R2:  code.emplace_back(0x36_byte); break;

			case ::Operand::R2_R0:  code.emplace_back(0x46_byte); break;
			case ::Operand::R2_R1:  code.emplace_back(0x56_byte); break;

			case ::Operand::R0_IMM: code.emplace_back(0x66_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_IMM: code.emplace_back(0x76_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_IMM: code.emplace_back(0x86_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::R0_MEM: code.emplace_back(0x96_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_MEM: code.emplace_back(0xA6_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_MEM: code.emplace_back(0xB6_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::MEM_R0: code.emplace_back(0xC6_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R1: code.emplace_back(0xD6_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R2: code.emplace_back(0xE6_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE(cmp)
		}
	}

	void do_move(std::vector<std::byte>& code, ::Operand type, const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		switch (type)
		{
			case ::Operand::R0_R1:  code.emplace_back(0x00_byte); break;
			case ::Operand::R0_R2:  code.emplace_back(0x10_byte); break;

			case ::Operand::R1_R0:  code.emplace_back(0x20_byte); break;
			case ::Operand::R1_R2:  code.emplace_back(0x30_byte); break;

			case ::Operand::R2_R0:  code.emplace_back(0x40_byte); break;
			case ::Operand::R2_R1:  code.emplace_back(0x50_byte); break;

			case ::Operand::R0_IMM: code.emplace_back(0x60_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_IMM: code.emplace_back(0x70_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_IMM: code.emplace_back(0x80_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::R0_MEM: code.emplace_back(0x90_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_MEM: code.emplace_back(0xA0_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_MEM: code.emplace_back(0xB0_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::MEM_R0: code.emplace_back(0xC0_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R1: code.emplace_back(0xD0_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R2: code.emplace_back(0xE0_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE(mov)
		}
	}

	void do_not(std::vector<std::byte>& code, ::Operand type, const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		switch (type)
		{
			case ::Operand::R0:  code.emplace_back(0x58_byte); break;
			case ::Operand::R1:  code.emplace_back(0x59_byte); break;
			case ::Operand::R2:  code.emplace_back(0x5A_byte); break;
			case ::Operand::MEM: code.emplace_back(0x5B_byte); embed_numeric<1_uz>(code, operands); break;
			DEFAULT_CASE(not)
		}
	}

	void do_and(std::vector<std::byte>& code, ::Operand type, const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		switch (type)
		{
			case ::Operand::R0_R1:  code.emplace_back(0x05_byte); break;
			case ::Operand::R0_R2:  code.emplace_back(0x15_byte); break;

			case ::Operand::R1_R0:  code.emplace_back(0x25_byte); break;
			case ::Operand::R1_R2:  code.emplace_back(0x35_byte); break;

			case ::Operand::R2_R0:  code.emplace_back(0x45_byte); break;
			case ::Operand::R2_R1:  code.emplace_back(0x55_byte); break;

			case ::Operand::R0_IMM: code.emplace_back(0x65_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_IMM: code.emplace_back(0x75_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_IMM: code.emplace_back(0x85_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::R0_MEM: code.emplace_back(0x95_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_MEM: code.emplace_back(0xA5_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_MEM: code.emplace_back(0xB5_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::MEM_R0: code.emplace_back(0xC5_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R1: code.emplace_back(0xD5_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R2: code.emplace_back(0xE5_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE(and)
		}
	}

	void do_xor(std::vector<std::byte>& code, ::Operand type, const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		switch (type)
		{
			case ::Operand::R0_R0:  code.emplace_back(0xF0_byte); break;
			case ::Operand::R1_R1:  code.emplace_back(0xF1_byte); break;
			case ::Operand::R2_R2:  code.emplace_back(0xF2_byte); break;

			case ::Operand::R0_R1:  code.emplace_back(0x03_byte); break;
			case ::Operand::R0_R2:  code.emplace_back(0x13_byte); break;

			case ::Operand::R1_R0:  code.emplace_back(0x23_byte); break;
			case ::Operand::R1_R2:  code.emplace_back(0x33_byte); break;

			case ::Operand::R2_R0:  code.emplace_back(0x43_byte); break;
			case ::Operand::R2_R1:  code.emplace_back(0x53_byte); break;

			case ::Operand::R0_IMM: code.emplace_back(0x63_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_IMM: code.emplace_back(0x73_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_IMM: code.emplace_back(0x83_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::R0_MEM: code.emplace_back(0x93_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_MEM: code.emplace_back(0xA3_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_MEM: code.emplace_back(0xB3_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::MEM_R0: code.emplace_back(0xC3_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R1: code.emplace_back(0xD3_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R2: code.emplace_back(0xE3_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE(xor)
		}
	}

	void do_or(std::vector<std::byte>& code, ::Operand type, const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		switch (type)
		{
			case ::Operand::R0_R1:  code.emplace_back(0x04_byte); break;
			case ::Operand::R0_R2:  code.emplace_back(0x14_byte); break;

			case ::Operand::R1_R0:  code.emplace_back(0x24_byte); break;
			case ::Operand::R1_R2:  code.emplace_back(0x34_byte); break;

			case ::Operand::R2_R0:  code.emplace_back(0x44_byte); break;
			case ::Operand::R2_R1:  code.emplace_back(0x54_byte); break;

			case ::Operand::R0_IMM: code.emplace_back(0x64_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_IMM: code.emplace_back(0x74_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_IMM: code.emplace_back(0x84_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::R0_MEM: code.emplace_back(0x94_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_MEM: code.emplace_back(0xA4_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_MEM: code.emplace_back(0xB4_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::MEM_R0: code.emplace_back(0xC4_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R1: code.emplace_back(0xD4_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R2: code.emplace_back(0xE4_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE(or)
		}
	}

	void do_add(std::vector<std::byte>& code, ::Operand type, const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		switch (type)
		{
			case ::Operand::R0_R1:  code.emplace_back(0x01_byte); break;
			case ::Operand::R0_R2:  code.emplace_back(0x11_byte); break;

			case ::Operand::R1_R0:  code.emplace_back(0x21_byte); break;
			case ::Operand::R1_R2:  code.emplace_back(0x31_byte); break;

			case ::Operand::R2_R0:  code.emplace_back(0x41_byte); break;
			case ::Operand::R2_R1:  code.emplace_back(0x51_byte); break;

			case ::Operand::R0_IMM: code.emplace_back(0x61_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_IMM: code.emplace_back(0x71_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_IMM: code.emplace_back(0x81_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::R0_MEM: code.emplace_back(0x91_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_MEM: code.emplace_back(0xA1_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_MEM: code.emplace_back(0xB1_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::MEM_R0: code.emplace_back(0xC1_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R1: code.emplace_back(0xD1_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R2: code.emplace_back(0xE1_byte); embed_numeric<1_uz>(code, operands); break;

			case ::Operand::SP_R0:  code.emplace_back(0x0A_byte); break;
			case ::Operand::SP_R1:  code.emplace_back(0x1A_byte); break;
			case ::Operand::SP_R2:  code.emplace_back(0x2A_byte); break;
			case ::Operand::SP_IMM: code.emplace_back(0x3A_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::SP_MEM: code.emplace_back(0x4A_byte); embed_numeric<2_uz>(code, operands); break;

			DEFAULT_CASE(add)
		}
	}

	void do_sub(std::vector<std::byte>& code, ::Operand type, const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		switch (type)
		{
			case ::Operand::R0_R1:  code.emplace_back(0x02_byte); break;
			case ::Operand::R0_R2:  code.emplace_back(0x12_byte); break;

			case ::Operand::R1_R0:  code.emplace_back(0x22_byte); break;
			case ::Operand::R1_R2:  code.emplace_back(0x32_byte); break;

			case ::Operand::R2_R0:  code.emplace_back(0x42_byte); break;
			case ::Operand::R2_R1:  code.emplace_back(0x52_byte); break;

			case ::Operand::R0_IMM: code.emplace_back(0x62_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_IMM: code.emplace_back(0x72_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_IMM: code.emplace_back(0x82_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::R0_MEM: code.emplace_back(0x92_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_MEM: code.emplace_back(0xA2_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_MEM: code.emplace_back(0xB2_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::MEM_R0: code.emplace_back(0xC2_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R1: code.emplace_back(0xD2_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R2: code.emplace_back(0xE2_byte); embed_numeric<1_uz>(code, operands); break;

			case ::Operand::SP_R0:  code.emplace_back(0x0B_byte); break;
			case ::Operand::SP_R1:  code.emplace_back(0x1B_byte); break;
			case ::Operand::SP_R2:  code.emplace_back(0x2B_byte); break;
			case ::Operand::SP_IMM: code.emplace_back(0x3B_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::SP_MEM: code.emplace_back(0x4B_byte); embed_numeric<2_uz>(code, operands); break;

			DEFAULT_CASE(sub)
		}
	}

	void do_rotate_left(std::vector<std::byte>& code, ::Operand type, const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		switch (type)
		{
			case ::Operand::R0_R1:  code.emplace_back(0x0D_byte); break;
			case ::Operand::R0_R2:  code.emplace_back(0x1D_byte); break;

			case ::Operand::R1_R0:  code.emplace_back(0x2D_byte); break;
			case ::Operand::R1_R2:  code.emplace_back(0x3D_byte); break;

			case ::Operand::R2_R0:  code.emplace_back(0x4D_byte); break;
			case ::Operand::R2_R1:  code.emplace_back(0x5D_byte); break;

			case ::Operand::R0_IMM: code.emplace_back(0x6D_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_IMM: code.emplace_back(0x7D_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_IMM: code.emplace_back(0x8D_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::R0_MEM: code.emplace_back(0x9D_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_MEM: code.emplace_back(0xAD_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_MEM: code.emplace_back(0xBD_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::MEM_R0: code.emplace_back(0xCD_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R1: code.emplace_back(0xDD_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R2: code.emplace_back(0xED_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE(rol)
		}
	}

	void do_rotate_right(std::vector<std::byte>& code, ::Operand type, const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		switch (type)
		{
			case ::Operand::R0_R1:  code.emplace_back(0x0C_byte); break;
			case ::Operand::R0_R2:  code.emplace_back(0x1C_byte); break;

			case ::Operand::R1_R0:  code.emplace_back(0x2C_byte); break;
			case ::Operand::R1_R2:  code.emplace_back(0x3C_byte); break;

			case ::Operand::R2_R0:  code.emplace_back(0x4C_byte); break;
			case ::Operand::R2_R1:  code.emplace_back(0x5C_byte); break;

			case ::Operand::R0_IMM: code.emplace_back(0x6C_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_IMM: code.emplace_back(0x7C_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_IMM: code.emplace_back(0x8C_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::R0_MEM: code.emplace_back(0x9C_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_MEM: code.emplace_back(0xAC_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_MEM: code.emplace_back(0xBC_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::MEM_R0: code.emplace_back(0xCC_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R1: code.emplace_back(0xDC_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R2: code.emplace_back(0xEC_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE(ror)
		}
	}

	void do_shift_left(std::vector<std::byte>& code, ::Operand type, const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		switch (type)
		{
			case ::Operand::R0_R1:  code.emplace_back(0x0F_byte); break;
			case ::Operand::R0_R2:  code.emplace_back(0x1F_byte); break;

			case ::Operand::R1_R0:  code.emplace_back(0x2F_byte); break;
			case ::Operand::R1_R2:  code.emplace_back(0x3F_byte); break;

			case ::Operand::R2_R0:  code.emplace_back(0x4F_byte); break;
			case ::Operand::R2_R1:  code.emplace_back(0x5F_byte); break;

			case ::Operand::R0_IMM: code.emplace_back(0x6F_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_IMM: code.emplace_back(0x7F_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_IMM: code.emplace_back(0x8F_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::R0_MEM: code.emplace_back(0x9F_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_MEM: code.emplace_back(0xAF_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_MEM: code.emplace_back(0xBF_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::MEM_R0: code.emplace_back(0xCF_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R1: code.emplace_back(0xDF_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R2: code.emplace_back(0xEF_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE(shl)
		}
	}

	void do_shift_right(std::vector<std::byte>& code, ::Operand type, const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		switch (type)
		{
			case ::Operand::R0_R1:  code.emplace_back(0x0E_byte); break;
			case ::Operand::R0_R2:  code.emplace_back(0x1E_byte); break;

			case ::Operand::R1_R0:  code.emplace_back(0x2E_byte); break;
			case ::Operand::R1_R2:  code.emplace_back(0x3E_byte); break;

			case ::Operand::R2_R0:  code.emplace_back(0x4E_byte); break;
			case ::Operand::R2_R1:  code.emplace_back(0x5E_byte); break;

			case ::Operand::R0_IMM: code.emplace_back(0x6E_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_IMM: code.emplace_back(0x7E_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_IMM: code.emplace_back(0x8E_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::R0_MEM: code.emplace_back(0x9E_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R1_MEM: code.emplace_back(0xAE_byte); embed_numeric<2_uz>(code, operands); break;
			case ::Operand::R2_MEM: code.emplace_back(0xBE_byte); embed_numeric<2_uz>(code, operands); break;

			case ::Operand::MEM_R0: code.emplace_back(0xCE_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R1: code.emplace_back(0xDE_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM_R2: code.emplace_back(0xEE_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE(shr)
		}
	}

	auto patch_jump(auto& code) noexcept
	{
		code.emplace_back(0xFF_byte);
		code.emplace_back(0xFF_byte);
	}

	void do_call(std::vector<std::byte>& code, ::Operand type) noexcept
	{
		if (type == ::Operand::NONE)
		{
			code.emplace_back(0x47_byte);
			patch_jump(code);
		}
		
		else
		{
			std::cerr << "Incorrect argument type passed to instruction call";
			std::exit(13);
		}
	}

	void do_jmp(std::vector<std::byte>& code, ::Operand type) noexcept
	{
		if (type == ::Operand::NONE)
		{
			code.emplace_back(0x37_byte);
			patch_jump(code);
		}

		else
		{
			std::cerr << "Incorrect argument type passed to instruction jmp";
			std::exit(13);
		}
	}

	void do_je(std::vector<std::byte>& code, ::Operand type) noexcept
	{
		if (type == ::Operand::NONE)
		{
			code.emplace_back(0x69_byte);
			patch_jump(code);
		}

		else
		{
			std::cerr << "Incorrect argument type passed to instruction je";
			std::exit(13);
		}
	}

	void do_jo(std::vector<std::byte>& code, ::Operand type) noexcept
	{
		if (type == ::Operand::NONE)
		{
			code.emplace_back(0x6A_byte);
			patch_jump(code);
		}

		else
		{
			std::cerr << "Incorrect argument type passed to instruction jo";
			std::exit(13);
		}
	}

	void do_jc(std::vector<std::byte>& code, ::Operand type) noexcept
	{
		if (type == ::Operand::NONE)
		{
			code.emplace_back(0x7A_byte);
			patch_jump(code);
		}

		else
		{
			std::cerr << "Incorrect argument type passed to instruction jc";
			std::exit(13);
		}
	}

	void do_jl(std::vector<std::byte>& code, ::Operand type) noexcept
	{
		if (type == ::Operand::NONE)
		{
			code.emplace_back(0x67_byte);
			patch_jump(code);
		}

		else
		{
			std::cerr << "Incorrect argument type passed to instruction jl";
			std::exit(13);
		}
	}

	void do_jg(std::vector<std::byte>& code, ::Operand type) noexcept
	{
		if (type == ::Operand::NONE)
		{
			code.emplace_back(0x68_byte);
			patch_jump(code);
		}

		else
		{
			std::cerr << "Incorrect argument type passed to instruction jg";
			std::exit(13);
		}
	}

	void do_jle(std::vector<std::byte>& code, ::Operand type) noexcept
	{
		if (type == ::Operand::NONE)
		{
			code.emplace_back(0x77_byte);
			patch_jump(code);
		}

		else
		{
			std::cerr << "Incorrect argument type passed to instruction jle";
			std::exit(13);
		}
	}

	void do_jge(std::vector<std::byte>& code, ::Operand type) noexcept
	{
		if (type == ::Operand::NONE)
		{
			code.emplace_back(0x78_byte);
			patch_jump(code);
		}

		else
		{
			std::cerr << "Incorrect argument type passed to instruction jge";
			std::exit(13);
		}
	}

	void do_jne(std::vector<std::byte>& code, ::Operand type) noexcept
	{
		if (type == ::Operand::NONE)
		{
			code.emplace_back(0x79_byte);
			patch_jump(code);
		}

		else
		{
			std::cerr << "Incorrect argument type passed to instruction jne";
			std::exit(13);
		}
	}

	void do_jno(std::vector<std::byte>& code, ::Operand type) noexcept
	{
		if (type == ::Operand::NONE)
		{
			code.emplace_back(0x6B_byte);
			patch_jump(code);
		}

		else
		{
			std::cerr << "Incorrect argument type passed to instruction jno";
			std::exit(13);
		}
	}

	void do_jnc(std::vector<std::byte>& code, ::Operand type) noexcept
	{
		if (type == ::Operand::NONE)
		{
			code.emplace_back(0x7B_byte);
			patch_jump(code);
		}

		else
		{
			std::cerr << "Incorrect argument type passed to instruction jnc";
			std::exit(13);
		}
	}

	void do_push(std::vector<std::byte>& code, ::Operand type, const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		switch (type)
		{
			case ::Operand::R0:    code.emplace_back(0x08_byte); break;
			case ::Operand::R1:    code.emplace_back(0x18_byte); break;
			case ::Operand::R2:    code.emplace_back(0x28_byte); break;
			case ::Operand::IMM:   code.emplace_back(0x38_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::MEM:   code.emplace_back(0x48_byte); embed_numeric<1_uz>(code, operands); break;
			case ::Operand::FLAGS: code.emplace_back(0xF6_byte); break;
			DEFAULT_CASE(push)
		}
	}

	void do_pop(std::vector<std::byte>& code, ::Operand type, const std::array<std::ssub_match, 3_uz>& operands) noexcept
	{
		switch (type)
		{
			case ::Operand::R0:      code.emplace_back(0x09_byte); break;
			case ::Operand::R1:      code.emplace_back(0x19_byte); break;
			case ::Operand::R2:      code.emplace_back(0x29_byte); break;
			case ::Operand::DISCARD: code.emplace_back(0x39_byte); break;
			case ::Operand::FLAGS:   code.emplace_back(0xF7_byte); break;
			case ::Operand::MEM:     code.emplace_back(0x49_byte); embed_numeric<1_uz>(code, operands); break;
				DEFAULT_CASE(pop)
		}
	}
#endif
#pragma endregion
}

//takes in assembly source and returns raw binary
std::vector<std::byte> cpu::assemble(std::string source) noexcept
{
	//retrieve the contents of each line of source code
	std::vector<std::string> lines = ::split(source, "\n", true);

	//will be later populated with all the labels used in the program
	std::map<std::string, std::size_t> labels;


	//deferrred label processing cachce
	std::vector<std::pair<std::string, std::size_t>> pending;

	//create a dynamically allocated array for storing machine code
	std::vector<std::byte> code;

	//https://regex101.com/r/NsOSMw/1
	std::regex labelRegex(R"(^(?:([A-z]\w*):\s*(?:;(?:.*))?))");

	//preprocessing pass--remove blank and comment lines
	//regex code source can be found here: https://regex101.com/r/APnEKf/1
	lines.erase(std::remove_if(lines.begin(), lines.end(), [](const std::string& str)
		{ return std::regex_match(str, std::regex(R"(^(\s*(?:;.*)?)$)")); }), lines.end());

	//preprocessing pass--remove leading whitespace
	for (auto& line : lines)
	{
		//create an iterator that points to the first non-whitespace character
		auto it = std::find_if(line.begin(), line.end(),
			[](unsigned char c) { return !std::isspace(c); });

		//erase all characters that exist before the created iterator
		line.erase(line.begin(), it);
	}

	//second pass--compile the source code
	for (auto&& line : lines)
	{
		std::smatch matches;
		if (std::regex_match(line, matches, labelRegex)) [[unlikely]]
		{
			if (labels.contains(matches[1].str()))
			{
				std::cerr << "Label resolution was ambiguous for duplicate label " << matches[1].str();
				std::exit(14);
			}

			else
			{
				labels.insert({ matches[1].str(), code.size() });
				continue;
			}
		}

		auto f = find_operands(line);
		auto o = get_encoding_type(f);

		switch (line[0])
		{
			case 'e':
				//ei
				code.emplace_back(0xFE_byte);
				break;

			case 'd':
				switch (line[1])
				{
					case 'i':
						//di
						code.emplace_back(0xFF_byte);
						break;

					case 'e':
						::do_decrement(code, o, f);
						break;
				}
				break;

			case 'c':
				switch (line[1])
				{
					case 'm':
						::do_cmp(code, o, f);
						break;

					case 'a':
					{
						::do_call(code, o);
						auto a = std::pair{ f[1], (code.size() - 2_uz) };
						pending.emplace_back(a);
						break;
					}
				}
				break;

			case 'n':
				::do_not(code, o, f);
				break;

			case 'm':
				::do_move(code, o, f);
				break;

			case 'x':
				::do_xor(code, o, f);
				break;

			case 'o':
				::do_or(code, o, f);
				break;

			case 'a':
				switch (line[1])
				{
					case 'd':
						::do_add(code, o, f);
						break;

					case 'n':
						::do_and(code, o, f);
						break;
				}
				break;

			case 'r':
				switch (line[1])
				{
					case 'e':
						switch (line[2])
						{
							case 's':
								//reset
								code.emplace_back(0xF5_byte);
								break;

							case 't':
								//return
								code.emplace_back(0x57_byte);
								break;
						}
						break;


					case 'o':
						switch (line[2])
						{
							case 'l':
								::do_rotate_left(code, o, f);
								break;

							case 'r':
								::do_rotate_right(code, o, f);
								break;
						}
						break;
				}
				break;

			case 's':
				switch (line[1])
				{
					case 'w':
						//swint
						code.emplace_back(0xF3_byte);
						break;

					case 'u':
						::do_sub(code, o, f);
						break;

					case 'h':
						switch (line[2])
						{
							case 'l':
								::do_shift_left(code, o, f);
								break;

							case 'r':
								::do_shift_right(code, o, f);
								break;
						}
						break;
				}
				break;

			case 'i':
				switch (line[2])
				{
					case 'c':
						::do_increment(code, o, f);
						break;

					case 't':
						//intret
						code.emplace_back(0xF4_byte);
						break;
				}
				break;

			case 'j':
				switch (line[1])
				{
					case 'm':
					{
						::do_jmp(code, o);
						auto a = std::pair{ f[1], (code.size() - 2_uz) };
						pending.emplace_back(a);
						break;
					}

					case 'e':
					{
						::do_je(code, o);
						auto a = std::pair{ f[1], (code.size() - 2_uz) };
						pending.emplace_back(a);
						break;
					}

					case 'o':
					{
						::do_jo(code, o);
						auto a = std::pair{ f[1], (code.size() - 2_uz) };
						pending.emplace_back(a);
						break;
					}

					case 'c':
					{
						::do_jc(code, o);
						auto a = std::pair{ f[1], (code.size() - 2_uz) };
						pending.emplace_back(a);
						break;
					}

					case 'l':
					{
						if (line[2] == 'e') ::do_jle(code, o);
						else ::do_jl(code, o);
						auto a = std::pair{ f[1], (code.size() - 2_uz) };
						pending.emplace_back(a);
						break;
					}

					case 'g':
					{
						if (line[2] == 'e') ::do_jge(code, o);
						else ::do_jg(code, o);
						auto a = std::pair{ f[1], (code.size() - 2_uz) };
						pending.emplace_back(a);
						break;
					}

					case 'n':
						switch (line[2])
						{
							case 'e':
							{
								::do_jne(code, o);
								auto a = std::pair{ f[1], (code.size() - 2_uz) };
								pending.emplace_back(a);
								break;
							}

							case 'o':
							{
								::do_jno(code, o);
								auto a = std::pair{ f[1], (code.size() - 2_uz) };
								pending.emplace_back(a);
								break;
							}

							case 'c':
							{
								::do_jnc(code, o);
								auto a = std::pair{ f[1], (code.size() - 2_uz) };
								pending.emplace_back(a);
								break;
							}
						}
						break;
				}
				break;

			case 'p':
				switch (line[1])
				{
					case 'u':
						if (line[4] == 'a')
						{
							//pushall
							code.emplace_back(0xFC_byte);
							break;
						}

						::do_push(code, o, f);
						break;

					case 'o':
						if (line[3] == 'a')
						{
							//popall
							code.emplace_back(0xFD_byte);
							break;
						}

						::do_pop(code, o, f);
						break;
				}
				break;
		}
	}

	for (const auto& pendee : pending)
	{
		//extract the patch-pending information
		const auto& [label, addr] = pendee;

		//verify that the referenced label actually exists
		if (labels.contains(label)) [[likely]]
		{
			/*Cast the 64 bit address index down to a 16 bit address. This way we can get
			the upper and lower 8 bit indicies that can then be patched into the binary*/
			auto address = static_cast<std::uint16_t>(labels.at(label));

			//extract the 8-bit upper and lower halves
			auto upper = static_cast<std::uint8_t>((address & 0xFF00) >> 8);
			auto lower = static_cast<std::uint8_t>((address & 0x00FF) >> 0);

			//patch in the jump target information
			code[addr + 0_uz] = std::byte{ lower };
			code[addr + 1_uz] = std::byte{ upper };
		}

		//otherwise, throw an error if it doesn't
		else [[unlikely]]
		{
			std::cerr << "Resolution could not bind reference " << label << " to a patchable label";
			std::exit(15);
		}
	}
	
	//return the populated machine code array
	return code;
}