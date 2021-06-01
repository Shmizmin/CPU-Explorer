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
		//for (auto&& c : line)
		//{
		//	if (c == ';')
		//}

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