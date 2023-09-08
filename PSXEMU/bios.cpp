#include "bios.h"
#include <fstream>

Bios::Bios(std::string path)
{
	bios_data_ = new u8[BIOS_ADDR_SPACE_SIZE];
	std::ifstream file(path, std::ios::binary);
	if (!file) {
		std::cerr << "Error opening file "<< std::endl;
	}
	file.read(
		reinterpret_cast<char*>(bios_data_),//.data()),
		BIOS_ADDR_SPACE_SIZE);
	if (!file) {
		std::cerr << "Error reading file, could only read " 
			<< file.gcount() << " bytes" << std::endl;
	}
	file.close();
}

Bios::~Bios()
{
	delete bios_data_;
	bios_data_ = nullptr;
}

Bios::Bios(const Bios& bios)
{
	bios_data_ = new u8[BIOS_ADDR_SPACE_SIZE];
	memcpy(bios_data_, bios.bios_data_, BIOS_ADDR_SPACE_SIZE * sizeof(u8));
}

u32 Bios::load32(u32 offset)
{
	u32 data =
		(static_cast<u32>(bios_data_[offset + 0])) |
		(static_cast<u32>(bios_data_[offset + 1]) << 8) |
		(static_cast<u32>(bios_data_[offset + 2]) << 16) |
		(static_cast<u32>(bios_data_[offset + 3]) << 24);
	return data;
}
