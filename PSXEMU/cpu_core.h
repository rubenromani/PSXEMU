#pragma once
#include "types.h"
#include "interconnect.h"

#define N_GP_REG 32
#define INSTR_LENGTH 4

namespace CPU
{

	struct State
	{
		u32 pc;					// Program counter
		u32 hi;
		u32 lo;
		u32 regs[N_GP_REG];		// Registers
		u32 sr;					// Coprocessor0 reg12: Status Register
	};

	struct RegisterIdx
	{
		RegisterIdx(u32 value) : value(value) {};
		u32 value;
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
		RegisterIdx s()
		{
			return RegisterIdx((value >> 21) & 0x1f);
		}

		// Returns the target register index [20:16]
		RegisterIdx t()
		{
			return RegisterIdx((value >> 16) & 0x1f);
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
		RegisterIdx d()
		{
			return RegisterIdx((value >> 11) & 0x1f);
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

		// return the coprocessor opcode [25:21]
		u32 cop_opcode()
		{
			return (value >> 21) & 0x1f;
		}

	};

	class Core
	{
	private:
		State state_;
		Instruction next_instruction_ = Instruction(0x00000000); //NOP
		Interconnect interconnect_;

		u32 load32_(u32 address);
		void store32_(u32 address, u32 value);
		void branch(u32 offset);
		void decode_and_execute_(Instruction instruction);

		static const u32
			ins_lui_ = 0b001111,
			ins_ori_ = 0b001101,
			ins_sw_ = 0b101011,
			ins_addiu_ = 0b001001,
			ins_addi_ = 0b001000,
			ins_j_ = 0b000010,
			ins_bne_ = 0b000101,

			ins_spec_ = 0b000000,
			ins_sll_ = 0b000000,
			ins_or_ = 0b100101,

			ins_cop0_ = 0b010000,
			ins_mtc0_ = 0b00100;


		void exec_lui_(Instruction instruction);		// Load upper immediate
		void exec_ori_(Instruction instruction);		// Bitwise OR immediate
		void exec_sw_(Instruction instruction);			// Store Word
		void exec_addiu_(Instruction instruction);		// Add Immediate Unsigned
		void exec_addi_(Instruction instruction);		// Add Immediate
		void exec_j_(Instruction instruction);			// Jump to target stored in [25:0]
		void exec_bne_(Instruction instruction);		// Branch not equal

		void exec_spec_(Instruction instruction);		// Special function
		void exec_sll_(Instruction instruction);		// Shift left logical
		void exec_or_(Instruction instruction);		// Shift left logical

		void exec_cop0_(Instruction instruction);	// Instruction for the coprocessor 0
		void exec_mtc0_(Instruction instruction);	// 

	public:
		Core(Interconnect interconnect);
		void run_next_instruction();
		void set_reg(RegisterIdx reg_idx, u32 value);
		u32 get_reg(RegisterIdx reg_idx) const;
	};
}
