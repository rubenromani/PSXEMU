#pragma once
#include"types.h"
#include"address_map.h"
#include<array>
#include<iostream>


class Bios
{
private:
	//std::array<u8, BIOS_ADDR_SPACE_SIZE> bios_data_;
	u8* bios_data_;
public:
	Bios(std::string path);
	~Bios();
	Bios(const Bios& bios);
	u32 load32(u32 offset);
};

