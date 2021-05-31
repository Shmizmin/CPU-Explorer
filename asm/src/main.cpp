import common;
import filerator;

int __cdecl main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) noexcept
{
	auto buf = std::move(cpu::filerator(argv[1]));

	for (auto& b : buf)
	{
		//std::cout << b;
		printf("Num: %d\n", (int)b);
	}

	return 0;
}