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

		// Returns the function opcode [31:26]
		u8 function()
		{
			return value >> 26;
		}

		// Returns the source register index [25:21]
		u8 source_register()
		{
			return (value >> 21) & 0x1f;
		}

		// Returns the target register index [20:16]
		u8 target_register()
		{
			return (value >> 16) & 0x1f;
		}
		
		// Returns the immediate value [16:0]
		u32 immediate()
		{
			return value & 0x0000ffff;
		}

		// Returns the signed immediate value [16:0]
		u32 signed_immediate()
		{
			u32 addr = value & 0x0000ffff;
			return ((addr >> 15) == 0x0 ) ? 
				addr : addr | 0xffff0000;
		}

		// Returns register index in bits [15::11]
		u8 d()
		{
			return (value >> 11) & 0x1f;
		}

		// Returns the bits [5:0] of the instruction
		u8 subfuction()
		{
			return value & 0x3f;
		}

		// Shift immediate value are stored in bits [10:6]
		u8 shift()
		{
			return (value >> 6) & 0x1f;
		}

		// Jump target stored in bits [25:0]
		u32 imm_jump()
		{
			return value & 0x3ffffff;
		}

	};

	class Core
	{
	private:
		State state_;
		Instruction next_instruction_ = Instruction(0x00000000); //NOP
		Interconnect interconnect_;

		u32 load32_(u32 address);
		u32 store32_(u32 address, u32 value);
		void decode_and_execute_(Instruction instruction);

		static const u32
			ins_lui_ =	0b001111,
			ins_ori_ =	0b001101,
			ins_sw_ =	0b101011,
			ins_addiu_ =	0b001001,
			ins_j_		= 0b000010,
			ins_spec_ = 0b000000,
			ins_sll_ = 0b000000,
			ins_or_ = 0b100101;


		void exec_lui_(Instruction instruction);		// Load upper immediate
		void exec_ori_(Instruction instruction);		// Bitwise OR immediate
		void exec_sw_(Instruction instruction);			// Store Word
		void exec_addiu_(Instruction instruction);		// Add Immediate Unsigned
		void exec_j_(Instruction instruction);		// Jump to target stored in [25:0]
		void exec_spec_(Instruction instruction);		// Store Word
		void exec_sll_(Instruction instruction);		// Shift left logical
		void exec_or_(Instruction instruction);		// Shift left logical

	public:
		Core(Interconnect interconnect);
		void run_next_instruction();
		void set_reg(RegIdx reg_idx, u32 value);
		u32 get_reg(RegIdx reg_idx) const;
		void set_reg(u8 reg_idx, u32 value);
		u32 get_reg(u8 reg_idx) const;
	};
}
