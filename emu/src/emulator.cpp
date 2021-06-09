#include "emulator.hpp"

#include <Windows.h>

namespace
{

}

//open up the emulator window
void cpu::launch(void) noexcept
{
	MessageBoxA(NULL, "Hello World", "This is a test caption", MB_OK);

}