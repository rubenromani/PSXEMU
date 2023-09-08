#pragma once
#include "types.h"
#include "interconnect.h"

#define N_GP_REG 32
#define INSTR_LENGTH 4

namespace CPU
{

	struct State
	{
		u32 pc;
		u32 hi;
		u32 lo;
		u32 regs[N_GP_REG];
	};

	enum RegIdx
	{
		zero,
		at,
		v0,v1,
		a0,a1,a2,a3,
		t0,t1,t2,t3,t4,t5,t6,t7,
		s0,s1,s2,s3,s4,s5,s6,s7,
		t8,t9,
		k0,k8,
		gp,
		sp,
		fp,
		ra
	};

	struct Instruction
	{
		u32 value;
		Instruction(u32 val) :
			value(val) {}

		u8 function()
		{
			return value >> 26;
		}

		// Return the target register index [20:16]
		u8 target_register()
		{
			return (value >> 16) & 0x0000001f;
		}

		// Return the immediate value [16:0]
		u16 immediate()
		{
			return value & 0x0000ffff;
		}

	};

	class Core
	{
	private:
		State state_;
		Interconnect interconnect_;

		u32 load32_(u32 address);
		void decode_and_execute_(Instruction instruction);

		static const u32
			ins_lui_ = 0b001111;


		void exec_lui_(Instruction instruction);

	public:
		Core(Interconnect interconnect);
		void run_next_instruction();
		void set_reg(RegIdx reg_idx, u32 value);
		u32 get_reg(RegIdx reg_idx) const;
		void set_reg(u8 reg_idx, u32 value);
		u32 get_reg(u8 reg_idx) const;
	};
}
