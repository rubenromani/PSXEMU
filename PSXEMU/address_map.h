#pragma once
#define DEVICE_MAP(address, device_start_address, device_end_address) \
	((device_start_address <= address) && (device_end_address > address))

#define CPU_RESET_ADDRESS 0xbfc00000

#define BIOS_START_ADDRESS 0xbfc00000
#define BIOS_ADDR_SPACE_SIZE (512*1024)
#define BIOS_END_ADDRESS (BIOS_START_ADDRESS + BIOS_ADDR_SPACE_SIZE)

