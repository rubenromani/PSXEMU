#include "interconnect.h"

u32 Interconnect::redirect_load32_(u32 address)
{
	if (address % 4 != 0)
	{
		std::cerr << "Unaligned load32 memory address: " << 
			std::hex << address << std::endl;
	}

	if (DEVICE_MAP(address, BIOS_START_ADDRESS, BIOS_END_ADDRESS)) 
	{
		bios_.load32(address - BIOS_START_ADDRESS);
	}
	else
	{
		std::cerr << "Unable to map memory address" << std::endl;
		throw -1;
	}
}


Interconnect::Interconnect(Bios bios) :
	bios_{ bios }
{
	
}

u32 Interconnect::load32(u32 address)
{
	return redirect_load32_(address);
}

u32 Interconnect::store32(u32 address, u32 value)
{
	if (address % 4 != 0)
	{
		std::cerr << "Unaligned store32 memory address: " <<
			std::hex << address << std::endl;
	}

	if (DEVICE_MAP(address, MEMCONTROL_START_ADDRESS, MEMCONTROL_END_ADDRESS)) 
	{
		switch (address - MEMCONTROL_START_ADDRESS)
		{
		case 0:
			if (value != 0x1f000000)
			{
				std::cerr << "Bad expansion 1 base address: " <<
					std::hex << address << std::endl;
				return 0x0;
			}
		case 4:
			if (value != 0x1f802000)
			{
				std::cerr << "Bad expansion 2 base address: " <<
					std::hex << address << std::endl;
				return 0x0;
			}
		default:
			std::cerr << "Unable to write to MEMCONTROL register" << std::endl;
			break;
		}
		return 0x00;
	}

	if (address == RAM_SIZE_LOCATION)
	{
		return 0x0;
	}


	std::cerr << "Unable to store in address: " <<
		std::hex << address << std::endl;
	throw - 1;
}
