#include "ram.h"
#include <string>

Ram::Ram()
{
	ram_data_ = new u8[RAM_ADDR_SPACE_SIZE];
	memset(ram_data_, 0xca, RAM_ADDR_SPACE_SIZE);
}

Ram::~Ram()
{
	delete ram_data_;
	ram_data_ = nullptr;
}

Ram::Ram(const Ram& ram)
{
	ram_data_ = new u8[RAM_ADDR_SPACE_SIZE];
	memcpy(ram_data_, ram.ram_data_, RAM_ADDR_SPACE_SIZE * sizeof(u8));
}

u32 Ram::load32(u32 offset)
{
	u32 data =
		(static_cast<u32>(ram_data_[offset + 0])) |
		(static_cast<u32>(ram_data_[offset + 1]) << 8) |
		(static_cast<u32>(ram_data_[offset + 2]) << 16) |
		(static_cast<u32>(ram_data_[offset + 3]) << 24);
	return data;
}

u8 Ram::load8(u32 offset)
{
	return ram_data_[offset];
}

void Ram::store32(u32 offset, u32 value)
{
	u8 b0 = static_cast<u8>(value >> 0);
	u8 b1 = static_cast<u8>(value >> 8);
	u8 b2 = static_cast<u8>(value >> 16);
	u8 b3 = static_cast<u8>(value >> 24);

	ram_data_[offset + 0] = b0;
	ram_data_[offset + 1] = b1;
	ram_data_[offset + 2] = b2;
	ram_data_[offset + 3] = b3;
}

void Ram::store8(u32 offset, u8 value)
{
	ram_data_[offset] = value;
}
