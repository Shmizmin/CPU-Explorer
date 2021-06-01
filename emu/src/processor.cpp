import processor;

//halts execution and resets state
void cpu::Processor::reset(void) noexcept
{
	//put known good values in each register
	R0 = 0x0000;
	R1 = 0x0000;
	R2 = 0x0000;
	R3 = 0x0000;
	SP = 0xFFF0;

	/*Read the low and high bytes of the reset vector from 
	memory, and compose them into a single 16-bit value*/
	IP = 0xFFF1;
	auto lower = static_cast<std::uint16_t>(mem.read(IP));
	++IP;
	auto upper = static_cast<std::uint16_t>(mem.read(IP));

	//unconditionally jump to that location
	IP = (lower | (upper << 8));
}

//run for each clock cycle of the system
void cpu::Processor::clock(void) noexcept
{

}