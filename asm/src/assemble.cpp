import assemble;

//internally linked functions
namespace
{
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
		NONE,
	};

	//extract operand from a line into a 3 size array of string
	auto find_operands(const std::string& line) noexcept
	{
		//https://regex101.com/r/uhtAvu/1
		std::regex insn(R"((?:([a-z]*) ((?:r[0-9])|(?:(?:#|%)[0-9]+))?(?:, )?((?:r[0-9])|(?:(?:#|%)[0-9]+))(?: )*(?:;(?:.*))?)$)");
		std::smatch matches;
		if (!std::regex_match(line, matches, insn))
		{
			std::cerr << "Assembly Syntax Error";
			std::exit(12);
		}

		//assembly is likely valid so return the matches we made
		else
		{
			return std::array{ matches[1], matches[2], matches[3] };
		}
	}

	//extract operand encoding type
	auto get_encoding_type(auto operands) noexcept
	{
		const auto&[instruction, op1, op2] = operands;
		auto encoding = ::Operands::NONE;
		using enum Operands;

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

#pragma warning(push)
#pragma warning(disable: 4100)
	auto do_decrement(const std::string& line) noexcept
	{

	}

	auto do_cmp(const std::string& line) noexcept
	{

	}

	auto do_call(const std::string& line) noexcept
	{

	}

	auto do_move(const std::string& line) noexcept
	{

	}

	auto do_not(const std::string& line) noexcept
	{

	}

	auto do_and(const std::string& line) noexcept
	{

	}


	auto do_xor(const std::string& line) noexcept
	{

	}

	auto do_or(const std::string& line) noexcept
	{

	}

	auto do_out(const std::string& line) noexcept
	{

	}

	auto do_add(const std::string& line) noexcept
	{

	}

	auto do_sub(const std::string& line) noexcept
	{

	}

	auto do_rotate_left(const std::string& line) noexcept
	{

	}

	auto do_rotate_right(const std::string& line) noexcept
	{

	}

	auto do_shift_left(const std::string& line) noexcept
	{

	}

	auto do_shift_right(const std::string& line) noexcept
	{

	}

	auto do_increment(const std::string& line) noexcept
	{

	}

	auto do_jmp(const std::string& line) noexcept
	{

	}

	auto do_je(const std::string& line) noexcept
	{

	}

	auto do_jo(const std::string& line) noexcept
	{

	}

	auto do_jc(const std::string& line) noexcept
	{

	}

	auto do_jl(const std::string& line) noexcept
	{

	}

	auto do_jg(const std::string& line) noexcept
	{

	}

	auto do_jle(const std::string& line) noexcept
	{

	}

	auto do_jge(const std::string& line) noexcept
	{

	}

	auto do_jne(const std::string& line) noexcept
	{

	}

	auto do_jno(const std::string& line) noexcept
	{

	}

	auto do_jnc(const std::string& line) noexcept
	{

	}

	auto do_push(const std::string& line) noexcept
	{

	}

	auto do_pop(const std::string& line) noexcept
	{

	}
#pragma warning(pop)
}

//takes in assembly source and returns raw binary
std::vector<std::byte> cpu::assemble(std::string source) noexcept
{
	//retrieve the contents of each line of source code
	const std::vector<std::string> lines = ::split(source, "\n", true);

	//create a dynamically allocated array for storing machine code
	std::vector<std::byte> code;

	//iterate over each line of source code
	for (auto&& line : lines)
	{
		auto f = find_operands(line);


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
					code.emplace_back(0xEF_byte);
					break;

				case 'e':
					do_decrement(line);
					break;
			}
			break;

		case 'c':
			switch (line[1])
			{
				case 'm':
					do_cmp(line);
					break;

				case 'a':
					do_call(line);
					break;
			}
			break;
			
		case 'n':
			do_not(line);
			break;

		case 'm':
			do_move(line);
			break;

		case 'x':
			do_xor(line);
			break;

		case 'o':
			switch (line[1])
			{
			case 'u':
				do_out(line);
				break;

			case 'r':
				do_or(line);
				break;
			}
			break;

		case 'a':
			switch (line[1])
			{
				case 'd':
					do_add(line);
					break;

				case 'n':
					do_and(line);
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
							code.emplace_back(0xE5_byte);
							break;

						case 'e':
							//return
							code.emplace_back(0x57_byte);
							break;
					}
					break;
					

				case 'o':
					switch (line[2])
					{
						case 'l':
							do_rotate_left(line);
							break;

						case 'r':
							do_rotate_right(line);
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
					code.emplace_back(0xE3_byte);
					break;

				case 'u':
					do_sub(line);
					break;

				case 'h':
					switch (line[2])
					{
						case 'l':
							do_shift_left(line);
							break;

						case 'r':
							do_shift_right(line);
							break;
					}
					break;
			}
			break;

		case 'i':
			switch (line[2])
			{
				case 'c':
					do_increment(line);
					break;

				case 't':
					//intret
					code.emplace_back(0xE4_byte);
					break;
			}
			break;

		case 'j':
			switch (line[1])
			{
				case 'm':
					do_jmp(line);
					break;

				case 'e':
					do_je(line);
					break;

				case 'o':
					do_jo(line);
					break;

				case 'c':
					do_jc(line);
					break;

				case 'l':
					if (line[2] == 'e') do_jle(line);
					else do_jl(line);
					break;

				case 'g':
					if (line[2] == 'e') do_jge(line);
					else do_jg(line);
					break;

				case 'n':
					switch (line[2])
					{
						case 'e':
							do_jne(line);
							break;

						case 'o':
							do_jno(line);
							break;

						case 'c':
							do_jnc(line);
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
						code.emplace_back(0xED_byte);
						break;
					}

					do_push(line);
					break;

				case 'o':
					if (line[3] == 'a')
					{
						//popall
						code.emplace_back(0xEE_byte);
						break;
					}

					do_pop(line);
					break;
			}
			break;
		}
	}
	
	//return the populated machine code array
	return code;
}