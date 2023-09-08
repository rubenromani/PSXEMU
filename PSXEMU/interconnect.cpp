#include "interconnect.h"

u32 Interconnect::redirect_load32_(u32 address)
{
	if (DEVICE_MAP(address, BIOS_START_ADDRESS, BIOS_END_ADDRESS)) {
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
	
