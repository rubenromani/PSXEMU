#pragma once
#define REGISTER_SIZE 4

#define DEVICE_MAP(address, device_start_address, device_end_address) \
	((device_start_address <= address) && (device_end_address > address))

#define CPU_RESET_ADDRESS 0xbfc00000

#define BIOS_START_ADDRESS 0xbfc00000
#define BIOS_ADDR_SPACE_SIZE (512*1024)
#define BIOS_END_ADDRESS (BIOS_START_ADDRESS + BIOS_ADDR_SPACE_SIZE)

#define MEMCONTROL_START_ADDRESS 0x1f801000
#define MEMCONTROL_ADDR_SPACE_SIZE 36
#define MEMCONTROL_END_ADDRESS (MEMCONTROL_START_ADDRESS + MEMCONTROL_ADDR_SPACE_SIZE)

#define RAM_SIZE_LOCATION 0x1f801060

