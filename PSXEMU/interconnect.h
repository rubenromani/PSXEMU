#pragma once
#include "bios.h"


class Interconnect
{
private:

	Bios bios_;
	u32 redirect_load32_(u32 address);

public:
	Interconnect(Bios bios);
	u32 load32(u32 address);
	u32 store32(u32 address, u32 value);
};

