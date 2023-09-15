#pragma once
#include "types.h"

namespace CPU {

	struct Instruction
	{
		u32 value;
		Instruction(u32 val) :
			value(val) {}

		u32 function()
		{
			return value >> 26;
		}

		// Return the target register index [20:16]
		u32 target_register()
		{
			return (value >> 16) & 0x0000001f;
		}

		// Return the immediate value [16:0]
		u32 immediate()
		{
			return value & 0x0000ffff;
		}

	};

	static class InstructionSet
	{
	public:
		static const u32
			ins_lui = 0b001111;

		void exec_lui(Core& core);
	};
}
