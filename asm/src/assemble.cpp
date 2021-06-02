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
	enum class Operands
	{
		R0_R0,
		R0_R1,
		R0_R2,

		R1_R0,
		R1_R1,
		R1_R2,

		R2_R0,
		R2_R1,
		R2_R2,

		R0_IMM,
		R1_IMM,
		R2_IMM,

		R0_MEM,
		R1_MEM,
		R2_MEM,

		MEM_R0,
		MEM_R1,
		MEM_R2,

		R0,
		R1,
		R2,
		IMM,
		MEM,

		SP,
		IP,
		FLAGS,

		SP_R0,
		SP_R1,
		SP_R2,
		SP_IMM,
		SP_MEM,

		DISCARD,
		LABEL,
		NONE,
	};
	using enum Operands;

	//extract operand from a line into a 3 size array of string
	auto find_operands(const auto& line) noexcept
	{
		//https://regex101.com/r/JVIYVc/1
		std::regex insn(R"(^\s*(?:([a-z]*) ?((?:[a-z]\w*)|(?:r[0-9])|(?:(?:#|%)\$?\w+))?(?:, (r[0-9]|(?:(?:#|%)\$?\w+)))?(?: )*(?:;(?:.*))?)$)");

		std::smatch insn_matches;


		//a line can only be an instruction or a label
		if (!std::regex_match(line, insn_matches, insn))
		{
			std::cerr << "Assembly Syntax Error";
			std::exit(12);
		}

		//assembly is likely valid so return the matches we made
		else
		{
			return std::array{ insn_matches[1], insn_matches[2], insn_matches.size() == 4 ? insn_matches[3] : std::ssub_match{} };
		}
	}

	//extract numeric operands
	template<std::size_t index>
	std::uint16_t get_numeric_operand(const auto& operands) noexcept
	{
		//const auto& [instruction, op1, op2] = operands;
		//const auto& instruction = operands[0];
		const auto& op1 = operands[1];
		const auto& op2 = operands[2];
		

		std::string header = "0x";

		if constexpr (index == 1_uz)
		{
			if (op1.str()[1] == '$')
			{
				return static_cast<std::uint16_t>(std::stoul(header + op1.str().substr(2_uz), 0, 16));
			}

			else
			{
				return static_cast<std::uint16_t>(std::stoul(op1.str().substr(1_uz), 0, 10));
			}
		}

		else if constexpr (index == 2_uz)
		{
			if (op1.str()[1] == '$')
			{
				return static_cast<std::uint16_t>(std::stoul(header + op2.str().substr(2_uz), 0, 16));
			}

			else
			{
				return static_cast<std::uint16_t>(std::stoul(op1.str().substr(1_uz), 0, 10));
			}
		}

		else
		{
			static_assert(false, "get_numeric_index requires an index of 1 or 2");
			return 0_u16;
		}

	}

	auto extract_bits(auto& code, auto whole) noexcept
	{
		auto upper = static_cast<std::uint8_t>((whole & 0xFF00) >> 8);
		auto lower = static_cast<std::uint8_t>((whole & 0x00FF) >> 0);
		code.emplace_back(std::byte{ lower });
		code.emplace_back(std::byte{ upper });
	}
	
	template<std::size_t index>
	auto embed_numeric(auto& code, const auto& operands) noexcept
	{
		auto whole = get_numeric_operand<index>(operands);
		extract_bits(code, whole);
	}
	
	//extract operand encoding type
	auto get_encoding_type(auto operands) noexcept
	{
		//const auto& [instruction, op1, op2] = std::tie(operands[0], operands[1], operands[2]);
		//const auto& instruction = operands[0];
		const auto& op1 = operands[1];
		const auto& op2 = operands[2];
		auto encoding = ::Operands::NONE;

		if (op1.matched)
		{
			switch (op1.str()[0])
			{
			case 'r':
				if (!op2.matched)
				{
					switch (op1.str()[1])
					{
						case '0': encoding = R0; break;
						case '1': encoding = R1; break;
						case '2': encoding = R2; break;
					}
				}

				else
				{
					switch (op2.str()[0])
					{
						case 'r':
								 if (op1.str() == "r0" && op2.str() == "r0") encoding = R0_R0;
							else if (op1.str() == "r0" && op2.str() == "r1") encoding = R0_R1;
							else if (op1.str() == "r0" && op2.str() == "r2") encoding = R0_R2;

							else if (op1.str() == "r1" && op2.str() == "r0") encoding = R1_R0;
							else if (op1.str() == "r1" && op2.str() == "r1") encoding = R1_R1;
							else if (op1.str() == "r1" && op2.str() == "r2") encoding = R1_R2;

							else if (op1.str() == "r2" && op2.str() == "r0") encoding = R2_R0;
							else if (op1.str() == "r2" && op2.str() == "r1") encoding = R2_R1;
							else if (op1.str() == "r2" && op2.str() == "r2") encoding = R2_R2;

							break;

						case '#':
							switch (op1.str()[1])
							{
								case '0': encoding = R0_IMM; break;
								case '1': encoding = R1_IMM; break;
								case '2': encoding = R2_IMM; break;
							}
							break;

						case '%':
							switch (op1.str()[1])
							{
								case '0': encoding = R0_MEM; break;
								case '1': encoding = R1_MEM; break;
								case '2': encoding = R2_MEM; break;
							}
							break;
					}
				}
				break;

			case '#':
				if (!op2.matched)
				{
					encoding = IMM;
				}
				break;

			case '%':
				if (!op2.matched)
				{
					encoding = MEM;
				}

				else if (op2.str()[0] == 'r')
				{
					switch (op2.str()[1])
					{
						case '0': encoding = MEM_R0; break;
						case '1': encoding = MEM_R1; break;
						case '2': encoding = MEM_R2; break;
					}
				}
				break;

			case 'd':
				encoding = DISCARD;
				break;

			case 'i':
				encoding = IP;
				break;

			case 'f':
				encoding = FLAGS;
				break;

			case 'S':
				if (!op2.matched)
				{
					encoding = SP;
				}

				else
				{
					switch (op2.str()[0])
					{
					case 'r':
						switch (op2.str()[1])
						{
							case '0': encoding = SP_R0; break;
							case '1': encoding = SP_R1; break;
							case '2': encoding = SP_R2; break;
						}
						break;

					case '#':
						encoding = SP_IMM;
						break;

					case '%':
						encoding = SP_MEM;
						break;
					}
				}
				break;
			}
		}

		else
		{
			encoding = NONE;
		}

		return encoding;
	}

#pragma region INSNS

#ifndef DEFAULT_CASE
#define DEFAULT_CASE default:                                             \
	std::cerr << "Invalid operand types for the specified instruction\n"; \
	std::exit(13);                                                        \
	break;

	auto do_decrement(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0:  code.emplace_back(0xF8_byte); break;
			case R1:  code.emplace_back(0xF9_byte); break;
			case R2:  code.emplace_back(0xFA_byte); break;
			case MEM: code.emplace_back(0xFB_byte); embed_numeric<1_uz>(code, operands); break;
			DEFAULT_CASE
		}
	}

	auto do_increment(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0:  code.emplace_back(0xE8_byte); break;
			case R1:  code.emplace_back(0xE9_byte); break;
			case R2:  code.emplace_back(0xEA_byte); break;
			case MEM: code.emplace_back(0xEB_byte); embed_numeric<1_uz>(code, operands); break;
			DEFAULT_CASE
		}
	}

	auto do_cmp(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0_R1:  code.emplace_back(0x06_byte); break;
			case R0_R2:  code.emplace_back(0x16_byte); break;

			case R1_R0:  code.emplace_back(0x26_byte); break;
			case R1_R2:  code.emplace_back(0x36_byte); break;

			case R2_R0:  code.emplace_back(0x46_byte); break;
			case R2_R1:  code.emplace_back(0x56_byte); break;

			case R0_IMM: code.emplace_back(0x66_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_IMM: code.emplace_back(0x76_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_IMM: code.emplace_back(0x86_byte); embed_numeric<2_uz>(code, operands); break;

			case R0_MEM: code.emplace_back(0x96_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_MEM: code.emplace_back(0xA6_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_MEM: code.emplace_back(0xB6_byte); embed_numeric<2_uz>(code, operands); break;

			case MEM_R0: code.emplace_back(0xC6_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R1: code.emplace_back(0xD6_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R2: code.emplace_back(0xE6_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE
		}
	}

	auto do_call(auto& code, const auto& labels, const auto& operands) noexcept
	{
		code.emplace_back(0x47_byte);
		auto name = operands[1];
		auto val = labels.at(name);
		extract_bits(code, val);
	}

	auto do_move(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0_R1:  code.emplace_back(0x00_byte); break;
			case R0_R2:  code.emplace_back(0x10_byte); break;

			case R1_R0:  code.emplace_back(0x20_byte); break;
			case R1_R2:  code.emplace_back(0x30_byte); break;

			case R2_R0:  code.emplace_back(0x40_byte); break;
			case R2_R1:  code.emplace_back(0x50_byte); break;

			case R0_IMM: code.emplace_back(0x60_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_IMM: code.emplace_back(0x70_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_IMM: code.emplace_back(0x80_byte); embed_numeric<2_uz>(code, operands); break;

			case R0_MEM: code.emplace_back(0x90_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_MEM: code.emplace_back(0xA0_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_MEM: code.emplace_back(0xB0_byte); embed_numeric<2_uz>(code, operands); break;

			case MEM_R0: code.emplace_back(0xC0_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R1: code.emplace_back(0xD0_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R2: code.emplace_back(0xE0_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE
		}
	}

	auto do_not(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0:  code.emplace_back(0x58_byte); break;
			case R1:  code.emplace_back(0x59_byte); break;
			case R2:  code.emplace_back(0x5A_byte); break;
			case MEM: code.emplace_back(0x5B_byte); embed_numeric<1_uz>(code, operands); break;
			DEFAULT_CASE
		}
	}

	auto do_and(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0_R1:  code.emplace_back(0x05_byte); break;
			case R0_R2:  code.emplace_back(0x15_byte); break;

			case R1_R0:  code.emplace_back(0x25_byte); break;
			case R1_R2:  code.emplace_back(0x35_byte); break;

			case R2_R0:  code.emplace_back(0x45_byte); break;
			case R2_R1:  code.emplace_back(0x55_byte); break;

			case R0_IMM: code.emplace_back(0x65_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_IMM: code.emplace_back(0x75_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_IMM: code.emplace_back(0x85_byte); embed_numeric<2_uz>(code, operands); break;

			case R0_MEM: code.emplace_back(0x95_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_MEM: code.emplace_back(0xA5_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_MEM: code.emplace_back(0xB5_byte); embed_numeric<2_uz>(code, operands); break;

			case MEM_R0: code.emplace_back(0xC5_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R1: code.emplace_back(0xD5_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R2: code.emplace_back(0xE5_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE
		}
	}

	auto do_xor(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0_R0:  code.emplace_back(0xF0_byte); break;
			case R1_R1:  code.emplace_back(0xF1_byte); break;
			case R2_R2:  code.emplace_back(0xF2_byte); break;

			case R0_R1:  code.emplace_back(0x03_byte); break;
			case R0_R2:  code.emplace_back(0x13_byte); break;

			case R1_R0:  code.emplace_back(0x23_byte); break;
			case R1_R2:  code.emplace_back(0x33_byte); break;

			case R2_R0:  code.emplace_back(0x43_byte); break;
			case R2_R1:  code.emplace_back(0x53_byte); break;

			case R0_IMM: code.emplace_back(0x63_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_IMM: code.emplace_back(0x73_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_IMM: code.emplace_back(0x83_byte); embed_numeric<2_uz>(code, operands); break;

			case R0_MEM: code.emplace_back(0x93_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_MEM: code.emplace_back(0xA3_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_MEM: code.emplace_back(0xB3_byte); embed_numeric<2_uz>(code, operands); break;

			case MEM_R0: code.emplace_back(0xC3_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R1: code.emplace_back(0xD3_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R2: code.emplace_back(0xE3_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE
		}
	}

	auto do_or(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0_R1:  code.emplace_back(0x04_byte); break;
			case R0_R2:  code.emplace_back(0x14_byte); break;

			case R1_R0:  code.emplace_back(0x24_byte); break;
			case R1_R2:  code.emplace_back(0x34_byte); break;

			case R2_R0:  code.emplace_back(0x44_byte); break;
			case R2_R1:  code.emplace_back(0x54_byte); break;

			case R0_IMM: code.emplace_back(0x64_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_IMM: code.emplace_back(0x74_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_IMM: code.emplace_back(0x84_byte); embed_numeric<2_uz>(code, operands); break;

			case R0_MEM: code.emplace_back(0x94_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_MEM: code.emplace_back(0xA4_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_MEM: code.emplace_back(0xB4_byte); embed_numeric<2_uz>(code, operands); break;

			case MEM_R0: code.emplace_back(0xC4_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R1: code.emplace_back(0xD4_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R2: code.emplace_back(0xE4_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE
		}
	}

	auto do_add(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0_R1:  code.emplace_back(0x01_byte); break;
			case R0_R2:  code.emplace_back(0x11_byte); break;

			case R1_R0:  code.emplace_back(0x21_byte); break;
			case R1_R2:  code.emplace_back(0x31_byte); break;

			case R2_R0:  code.emplace_back(0x41_byte); break;
			case R2_R1:  code.emplace_back(0x51_byte); break;

			case R0_IMM: code.emplace_back(0x61_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_IMM: code.emplace_back(0x71_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_IMM: code.emplace_back(0x81_byte); embed_numeric<2_uz>(code, operands); break;

			case R0_MEM: code.emplace_back(0x91_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_MEM: code.emplace_back(0xA1_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_MEM: code.emplace_back(0xB1_byte); embed_numeric<2_uz>(code, operands); break;

			case MEM_R0: code.emplace_back(0xC1_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R1: code.emplace_back(0xD1_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R2: code.emplace_back(0xE1_byte); embed_numeric<1_uz>(code, operands); break;

			case SP_R0:  code.emplace_back(0x0A_byte); break;
			case SP_R1:  code.emplace_back(0x1A_byte); break;
			case SP_R2:  code.emplace_back(0x2A_byte); break;
			case SP_IMM: code.emplace_back(0x3A_byte); embed_numeric<2_uz>(code, operands); break;
			case SP_MEM: code.emplace_back(0x4A_byte); embed_numeric<2_uz>(code, operands); break;

			DEFAULT_CASE
		}
	}

	auto do_sub(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0_R1:  code.emplace_back(0x02_byte); break;
			case R0_R2:  code.emplace_back(0x12_byte); break;

			case R1_R0:  code.emplace_back(0x22_byte); break;
			case R1_R2:  code.emplace_back(0x32_byte); break;

			case R2_R0:  code.emplace_back(0x42_byte); break;
			case R2_R1:  code.emplace_back(0x52_byte); break;

			case R0_IMM: code.emplace_back(0x62_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_IMM: code.emplace_back(0x72_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_IMM: code.emplace_back(0x82_byte); embed_numeric<2_uz>(code, operands); break;

			case R0_MEM: code.emplace_back(0x92_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_MEM: code.emplace_back(0xA2_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_MEM: code.emplace_back(0xB2_byte); embed_numeric<2_uz>(code, operands); break;

			case MEM_R0: code.emplace_back(0xC2_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R1: code.emplace_back(0xD2_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R2: code.emplace_back(0xE2_byte); embed_numeric<1_uz>(code, operands); break;

			case SP_R0:  code.emplace_back(0x0B_byte); break;
			case SP_R1:  code.emplace_back(0x1B_byte); break;
			case SP_R2:  code.emplace_back(0x2B_byte); break;
			case SP_IMM: code.emplace_back(0x3B_byte); embed_numeric<2_uz>(code, operands); break;
			case SP_MEM: code.emplace_back(0x4B_byte); embed_numeric<2_uz>(code, operands); break;

			DEFAULT_CASE
		}
	}

	auto do_rotate_left(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0_R1:  code.emplace_back(0x0D_byte); break;
			case R0_R2:  code.emplace_back(0x1D_byte); break;

			case R1_R0:  code.emplace_back(0x2D_byte); break;
			case R1_R2:  code.emplace_back(0x3D_byte); break;

			case R2_R0:  code.emplace_back(0x4D_byte); break;
			case R2_R1:  code.emplace_back(0x5D_byte); break;

			case R0_IMM: code.emplace_back(0x6D_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_IMM: code.emplace_back(0x7D_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_IMM: code.emplace_back(0x8D_byte); embed_numeric<2_uz>(code, operands); break;

			case R0_MEM: code.emplace_back(0x9D_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_MEM: code.emplace_back(0xAD_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_MEM: code.emplace_back(0xBD_byte); embed_numeric<2_uz>(code, operands); break;

			case MEM_R0: code.emplace_back(0xCD_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R1: code.emplace_back(0xDD_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R2: code.emplace_back(0xED_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE
		}
	}

	auto do_rotate_right(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0_R1:  code.emplace_back(0x0C_byte); break;
			case R0_R2:  code.emplace_back(0x1C_byte); break;

			case R1_R0:  code.emplace_back(0x2C_byte); break;
			case R1_R2:  code.emplace_back(0x3C_byte); break;

			case R2_R0:  code.emplace_back(0x4C_byte); break;
			case R2_R1:  code.emplace_back(0x5C_byte); break;

			case R0_IMM: code.emplace_back(0x6C_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_IMM: code.emplace_back(0x7C_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_IMM: code.emplace_back(0x8C_byte); embed_numeric<2_uz>(code, operands); break;

			case R0_MEM: code.emplace_back(0x9C_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_MEM: code.emplace_back(0xAC_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_MEM: code.emplace_back(0xBC_byte); embed_numeric<2_uz>(code, operands); break;

			case MEM_R0: code.emplace_back(0xCC_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R1: code.emplace_back(0xDC_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R2: code.emplace_back(0xEC_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE
		}
	}

	auto do_shift_left(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0_R1:  code.emplace_back(0x0F_byte); break;
			case R0_R2:  code.emplace_back(0x1F_byte); break;

			case R1_R0:  code.emplace_back(0x2F_byte); break;
			case R1_R2:  code.emplace_back(0x3F_byte); break;

			case R2_R0:  code.emplace_back(0x4F_byte); break;
			case R2_R1:  code.emplace_back(0x5F_byte); break;

			case R0_IMM: code.emplace_back(0x6F_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_IMM: code.emplace_back(0x7F_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_IMM: code.emplace_back(0x8F_byte); embed_numeric<2_uz>(code, operands); break;

			case R0_MEM: code.emplace_back(0x9F_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_MEM: code.emplace_back(0xAF_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_MEM: code.emplace_back(0xBF_byte); embed_numeric<2_uz>(code, operands); break;

			case MEM_R0: code.emplace_back(0xCF_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R1: code.emplace_back(0xDF_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R2: code.emplace_back(0xEF_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE
		}
	}

	auto do_shift_right(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0_R1:  code.emplace_back(0x0E_byte); break;
			case R0_R2:  code.emplace_back(0x1E_byte); break;

			case R1_R0:  code.emplace_back(0x2E_byte); break;
			case R1_R2:  code.emplace_back(0x3E_byte); break;

			case R2_R0:  code.emplace_back(0x4E_byte); break;
			case R2_R1:  code.emplace_back(0x5E_byte); break;

			case R0_IMM: code.emplace_back(0x6E_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_IMM: code.emplace_back(0x7E_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_IMM: code.emplace_back(0x8E_byte); embed_numeric<2_uz>(code, operands); break;

			case R0_MEM: code.emplace_back(0x9E_byte); embed_numeric<2_uz>(code, operands); break;
			case R1_MEM: code.emplace_back(0xAE_byte); embed_numeric<2_uz>(code, operands); break;
			case R2_MEM: code.emplace_back(0xBE_byte); embed_numeric<2_uz>(code, operands); break;

			case MEM_R0: code.emplace_back(0xCE_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R1: code.emplace_back(0xDE_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM_R2: code.emplace_back(0xEE_byte); embed_numeric<1_uz>(code, operands); break;

			DEFAULT_CASE
		}
	}

	auto do_jmp(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0:  code.emplace_back(0x07_byte); break;
			case R1:  code.emplace_back(0x17_byte); break;
			case R2:  code.emplace_back(0x27_byte); break;
			case MEM: code.emplace_back(0x37_byte); embed_numeric<1_uz>(code, operands); break;
			DEFAULT_CASE
		}
	}

	auto do_je(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0:  code.emplace_back(0x69_byte); break;
			case R1:  code.emplace_back(0x79_byte); break;
			case R2:  code.emplace_back(0x89_byte); break;
			case MEM: code.emplace_back(0x99_byte); embed_numeric<1_uz>(code, operands); break;
			DEFAULT_CASE
		}
	}

	auto do_jo(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0:  code.emplace_back(0x6A_byte); break;
			case R1:  code.emplace_back(0x7A_byte); break;
			case R2:  code.emplace_back(0x8A_byte); break;
			case MEM: code.emplace_back(0x9A_byte); embed_numeric<1_uz>(code, operands); break;
			DEFAULT_CASE
		}
	}

	auto do_jc(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0:  code.emplace_back(0xAA_byte); break;
			case R1:  code.emplace_back(0xBA_byte); break;
			case R2:  code.emplace_back(0xCA_byte); break;
			case MEM: code.emplace_back(0xDA_byte); embed_numeric<1_uz>(code, operands); break;
			DEFAULT_CASE
		}
	}

	auto do_jl(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0:  code.emplace_back(0x67_byte); break;
			case R1:  code.emplace_back(0x77_byte); break;
			case R2:  code.emplace_back(0x87_byte); break;
			case MEM: code.emplace_back(0x97_byte); embed_numeric<1_uz>(code, operands); break;
			DEFAULT_CASE
		}
	}

	auto do_jg(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0:  code.emplace_back(0xA7_byte); break;
			case R1:  code.emplace_back(0xB7_byte); break;
			case R2:  code.emplace_back(0xC7_byte); break;
			case MEM: code.emplace_back(0xD7_byte); embed_numeric<1_uz>(code, operands); break;
			DEFAULT_CASE
		}
	}

	auto do_jle(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0:  code.emplace_back(0x68_byte); break;
			case R1:  code.emplace_back(0x78_byte); break;
			case R2:  code.emplace_back(0x88_byte); break;
			case MEM: code.emplace_back(0x98_byte); embed_numeric<1_uz>(code, operands); break;
			DEFAULT_CASE
		}
	}

	auto do_jge(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0:  code.emplace_back(0xA8_byte); break;
			case R1:  code.emplace_back(0xB8_byte); break;
			case R2:  code.emplace_back(0xC8_byte); break;
			case MEM: code.emplace_back(0xD8_byte); embed_numeric<1_uz>(code, operands); break;
			DEFAULT_CASE
		}
	}

	auto do_jne(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0:  code.emplace_back(0xA9_byte); break;
			case R1:  code.emplace_back(0xB9_byte); break;
			case R2:  code.emplace_back(0xC9_byte); break;
			case MEM: code.emplace_back(0xD9_byte); embed_numeric<1_uz>(code, operands); break;
			DEFAULT_CASE
		}
	}

	auto do_jno(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0:  code.emplace_back(0x6B_byte); break;
			case R1:  code.emplace_back(0x7B_byte); break;
			case R2:  code.emplace_back(0x8B_byte); break;
			case MEM: code.emplace_back(0x9B_byte); embed_numeric<1_uz>(code, operands); break;
			DEFAULT_CASE
		}
	}

	auto do_jnc(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0:  code.emplace_back(0xAB_byte); break;
			case R1:  code.emplace_back(0xBB_byte); break;
			case R2:  code.emplace_back(0xCB_byte); break;
			case MEM: code.emplace_back(0xDB_byte); embed_numeric<1_uz>(code, operands); break;
			DEFAULT_CASE
		}
	}

	auto do_push(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0:    code.emplace_back(0x08_byte); break;
			case R1:    code.emplace_back(0x18_byte); break;
			case R2:    code.emplace_back(0x28_byte); break;
			case IMM:   code.emplace_back(0x38_byte); embed_numeric<1_uz>(code, operands); break;
			case MEM:   code.emplace_back(0x48_byte); embed_numeric<1_uz>(code, operands); break;
			case FLAGS: code.emplace_back(0xF6_byte); break;
		}
	}

	auto do_pop(auto& code, auto type, const auto& operands) noexcept
	{
		switch (type)
		{
			case R0:      code.emplace_back(0x09_byte); break;
			case R1:      code.emplace_back(0x19_byte); break;
			case R2:      code.emplace_back(0x29_byte); break;
			case DISCARD: code.emplace_back(0x39_byte); break;
			case FLAGS:   code.emplace_back(0xF7_byte); break;
			case MEM:     code.emplace_back(0x49_byte); embed_numeric<1_uz>(code, operands); break;
		}
	}
#endif
#pragma endregion
}

//takes in assembly source and returns raw binary
std::vector<std::byte> cpu::assemble(std::string source) noexcept
{
	//retrieve the contents of each line of source code
	const std::vector<std::string> lines = ::split(source, "\n", true);

	std::map<std::string, std::size_t> labelMap;
	
	//create a dynamically allocated array for storing machine code
	std::vector<std::byte> code;

	//https://regex101.com/r/APnEKf/1
	std::regex blank(R"(^(\s*(?:;.*)?)$)");

	//https://regex101.com/r/NsOSMw/1
	std::regex labelRegex(R"(^(?:([A-z]\w*):\s*(?:;(?:.*))?))");

	//iterate over each line of source code
	for (auto&& lmod : lines)
	{
		auto my_isspace = [](char ch) noexcept
		{
			return std::isspace(static_cast<unsigned char>(ch));
		};

		auto i = 0_uz;
		while (my_isspace(lmod[i])) ++i;
		auto line = lmod.substr(i);

		if (std::regex_match(line, blank))
			continue;

		std::smatch matches;
		if (std::regex_match(line, matches, labelRegex))
		{
			labelMap.insert({ matches[1].str(), code.size() });
		}

		else
		{
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
							do_decrement(code, o, f);
							break;
					}
					break;

				case 'c':
					switch (line[1])
					{
						case 'm':
							do_cmp(code, o, f);
							break;

						case 'a':
							do_call(code, labelMap, f);
							break;
					}
					break;

				case 'n':
					do_not(code, o, f);
					break;

				case 'm':
					do_move(code, o, f);
					break;

				case 'x':
					do_xor(code, o, f);
					break;

				case 'o':
					do_or(code, o, f);
					break;

				case 'a':
					switch (line[1])
					{
						case 'd':
							do_add(code, o, f);
							break;

						case 'n':
							do_and(code, o, f);
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
									do_rotate_left(code, o, f);
									break;

								case 'r':
									do_rotate_right(code, o, f);
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
							do_sub(code, o, f);
							break;

						case 'h':
							switch (line[2])
							{
								case 'l':
									do_shift_left(code, o, f);
									break;

								case 'r':
									do_shift_right(code, o, f);
									break;
							}
							break;
					}
					break;

				case 'i':
					switch (line[2])
					{
						case 'c':
							do_increment(code, o, f);
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
							do_jmp(code, o, f);
							break;

						case 'e':
							do_je(code, o, f);
							break;

						case 'o':
							do_jo(code, o, f);
							break;

						case 'c':
							do_jc(code, o, f);
							break;

						case 'l':
							if (line[2] == 'e') do_jle(code, o, f);
							else do_jl(code, o, f);
							break;

						case 'g':
							if (line[2] == 'e') do_jge(code, o, f);
							else do_jg(code, o, f);
							break;

						case 'n':
							switch (line[2])
							{
								case 'e':
									do_jne(code, o, f);
									break;

								case 'o':
									do_jno(code, o, f);
									break;

								case 'c':
									do_jnc(code, o, f);
									break;
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

							do_push(code, o, f);
							break;

						case 'o':
							if (line[3] == 'a')
							{
								//popall
								code.emplace_back(0xFD_byte);
								break;
							}

							do_pop(code, o, f);
							break;
					}
					break;
			}
		}
	}
	
	//return the populated machine code array
	return code;
}