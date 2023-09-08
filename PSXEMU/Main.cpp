#include <iostream>
#include "cpu_core.h"


int main() 
{
	std::cout << "Hello there!" << std::endl;
	Bios bios = Bios("SCPH1001.BIN");
	Interconnect interconnect = Interconnect(bios);
	CPU::Core cpu_core = CPU::Core(interconnect);

	for (int i = 0; i < 100; i++)
	{
		cpu_core.run_next_instruction();
	}

	return 0;
}