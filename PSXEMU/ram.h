#pragma once
#include "types.h"
#include"address_map.h"

class Ram
{
private:
	u8* ram_data_;
public:
	Ram();
	~Ram();
	Ram(const Ram& ram);
	u32 load32(u32 offset);
	u8 load8(u32 offset);
	void store32(u32 offset, u32 value);
	void store8(u32 offset, u8 value);

};

