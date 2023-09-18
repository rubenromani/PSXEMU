#pragma once
#include "bios.h"
#include "ram.h"

class Interconnect
{
private:
	const u32 REGION_MASK[8] =
	{
		// KUSEG: 2048 MB
		0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
		// KSEG0 : 512 MB
		0x7fffffff,
		// KSEG1 : 512 MB
		0x1fffffff,
		// KSEG2: 1024 MB
		0xffffffff, 0xffffffff
	};

	Bios bios_;
	Ram ram_;

public:
	Interconnect(Bios bios);
	u32 load32(u32 address);
	u8 load8(u32 address);
	void store32(u32 address, u32 value);
	void store16(u32 address, u16 value);
	void store8(u32 address, u8 value);
	u32 mask_region(u32 address);
};

