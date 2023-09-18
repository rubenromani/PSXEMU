#include "interconnect.h"

Interconnect::Interconnect(Bios bios) :
	bios_{ bios }
{
	
}

u32 Interconnect::load32(u32 address)
{
	address = mask_region(address);

	if (address % 4 != 0)
	{
		std::cerr << "Unaligned load32 memory address: " <<
			std::hex << address << std::endl;
	}

	if (DEVICE_MAP(address, BIOS_START_ADDRESS, BIOS_END_ADDRESS))
	{
		return bios_.load32(address - BIOS_START_ADDRESS);
	}
	else if (DEVICE_MAP(address, RAM_START_ADDRESS, RAM_END_ADDRESS))
	{
		return ram_.load32(address - RAM_START_ADDRESS);
	}
	else if (DEVICE_MAP(address, IRQ_CONTROL_START_ADDRESS, IRQ_CONTROL_END_ADDRESS))
	{
		std::cout << "IRQ CONTROL load32: " <<
			std::hex << (address - IRQ_CONTROL_START_ADDRESS) << std::endl;
		return 0x0;
	}
	else
	{
		std::cerr << "Unable to map memory address for load32" << std::endl;
		throw - 1;
	}
}

u8 Interconnect::load8(u32 address)
{
	address = mask_region(address);

	if (DEVICE_MAP(address, BIOS_START_ADDRESS, BIOS_END_ADDRESS))
	{
		return bios_.load8(address - BIOS_START_ADDRESS);
	}
	else if (DEVICE_MAP(address, EXPANSION1_START_ADDRESS, EXPANSION1_END_ADDRESS))
	{
		std::cout << "No Expantion 1 implementation" << std::endl;
		return 0xff;
	}
	else if (DEVICE_MAP(address, RAM_START_ADDRESS, RAM_END_ADDRESS))
	{
		return ram_.load8(address - RAM_START_ADDRESS);
	}
	else if (DEVICE_MAP(address, RAM2_START_ADDRESS, RAM2_END_ADDRESS))
	{
		//return ram_.load8(address - RAM2_START_ADDRESS);
		return 0x0;
	}
	else
	{
		std::cerr << "Unable to map memory address for load8, " << 
			"Address : " << std::hex << address <<std::endl;
		throw - 1;
	}

}

void Interconnect::store32(u32 address, u32 value)
{
	address = mask_region(address);

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
				return;
			}
		case 4:
			if (value != 0x1f802000)
			{
				std::cerr << "Bad expansion 2 base address: " <<
					std::hex << address << std::endl;
				return;
			}
		default:
			std::cerr << "Unable to write to MEMCONTROL register: " << std::hex << address << std::endl;
			break;
		}
		return;
	}
	else if (DEVICE_MAP(address, RAM_START_ADDRESS, RAM_END_ADDRESS))
	{
		ram_.store32(address - RAM_START_ADDRESS, value);
		return;
	}
	else if (DEVICE_MAP(address, IRQ_CONTROL_START_ADDRESS, IRQ_CONTROL_END_ADDRESS))
	{
		std::cout << "IRQ CONTROL store32: " <<
			std::hex << (address - IRQ_CONTROL_START_ADDRESS) <<
			" <- " << std::hex << value;
		return;
	}

	if (address == RAM_SIZE_LOCATION)
	{
		std::cout << "Unable to write in the RAM_SIZE_LOCATION" << std::endl;
		return;
	}

	if (address == CACHE_CONTROL)
	{
		std::cout << "Unable to write in the CACHE_CONTROL Register" << std::endl;
		return;
	}


	std::cerr << "Unable to store in address: " <<
		std::hex << address << std::endl;
	throw - 1;
}

void Interconnect::store16(u32 address, u16 value)
{
	address = mask_region(address);

	if ((address % 2) != 0)
	{
		std::cerr << "UNALIGNED STORE16 ADDRESS : " << 
			std::hex << address << std::endl;
		return;
	}

	if (DEVICE_MAP(address, SPU_START_ADDRESS, SPU_END_ADDRESS))
	{
		ram_.store32(address - SPU_START_ADDRESS, value);
		return;
	}
	else if (DEVICE_MAP(address, TIMERS_START_ADDRESS, TIMERS_END_ADDRESS))
	{
		std::cout << "Unhandles store16 into address (TIMERS REGISTER) : " <<
			std::hex << address << std::endl;
		return;
	}


	std::cerr << "Unhandles store16 into address : " <<
		std::hex << address << std::endl;
	throw - 1;

}

void Interconnect::store8(u32 address, u8 value)
{
	address = mask_region(address);

	if (DEVICE_MAP(address, EXPANSION2_START_ADDRESS, EXPANSION2_END_ADDRESS))
	{
		std::cout << "Unhandled write to Expansion 2 register: " <<
			std::hex << address << std::endl;
		return;
	}

	else if (DEVICE_MAP(address, RAM_START_ADDRESS, RAM_END_ADDRESS))
	{
		ram_.store8(address - RAM_START_ADDRESS, value);
		return;
	}


	std::cerr << "Unhandles store8 into address : " <<
		std::hex << address << std::endl;
	throw - 1;
}

u32 Interconnect::mask_region(u32 address)
{
	usize index = (address >> 29);
	return address & REGION_MASK[index];
}