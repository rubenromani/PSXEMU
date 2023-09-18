#pragma once
#include "types.h"
#include "interconnect.h"

#define N_GP_REG 32
#define INSTR_LENGTH 4

namespace CPU
{
	struct RegisterIdx
	{
		RegisterIdx(u32 value) : value(value) {};
		u32 value;
	};

	struct Cop0Regs
	{
		u32 bpc;		// 3
		u32 bda;		// 5	
		u32 _6;			// 6
		u32 dcic;		// 7
		u32 bdam;		// 9
		u32 bpcm;		// 11
		u32 sr;			// 12
		u32 cause;		// 13
	};

	struct State
	{
		u32 pc;					// Program counter
		u32 hi;
		u32 lo;
		u32 regs[N_GP_REG];		// Registers
		u32 out_regs[N_GP_REG];	// to handle load delay slot
		std::pair<RegisterIdx, u32> load = {RegisterIdx(0),0};

		Cop0Regs cop0regs;					// Coprocessor0 reg12: Status Register
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

		void copy_regs();
		u32 load32_(u32 address);
		u8 load8_(u32 address);
		void store32_(u32 address, u32 value);
		void store16_(u32 address, u16 value);
		void store8_(u32 address, u8 value);
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
			ins_lw_ = 0b100011,
			ins_sh_ = 0b101001,
			ins_jal_ = 0b000011,
			ins_sb_ = 0b101000,
			ins_andi_ = 0b001100,
			ins_lb_ = 0b100000,
			ins_beq_ = 0b000100,
			ins_bgtz_ = 0b000111,
			ins_blez_ = 0b000110,
			ins_lbu_ = 0b100100,
			ins_bxx_ = 0b000001,
			ins_slti_ = 0b001010,
			ins_sltiu_ = 0b001011,

			ins_spec_ = 0b000000,
			ins_sll_ = 0b000000,
			ins_or_ = 0b100101,
			ins_sltu_ = 0b101011,
			ins_addu_ = 0b100001,
			ins_jr_ = 0b001000,
			ins_and_ = 0b100100,
			ins_add_ = 0b100000,
			ins_jalr_ = 0b001001,
			ins_subu_ = 0b100011,
			ins_sra_ = 0b000011,
			ins_div_ = 0b011010,
			ins_mflo_ = 0b010010,
			ins_srl_ = 0b000010,
			ins_divu_ = 0b011011,
			ins_mfhi_ = 0b010000,
			ins_slt_ = 0b101010,

			ins_cop0_ = 0b010000,
			ins_mtc0_ = 0b00100,
			ins_mfc0_ = 0b00000;


		void exec_lui_(Instruction instruction);		// Load upper immediate
		void exec_ori_(Instruction instruction);		// Bitwise OR immediate
		void exec_sw_(Instruction instruction);			// Store Word
		void exec_addiu_(Instruction instruction);		// Add Immediate Unsigned
		void exec_addi_(Instruction instruction);		// Add Immediate
		void exec_j_(Instruction instruction);			// Jump to target stored in [25:0]
		void exec_bne_(Instruction instruction);		// Branch not equal
		void exec_lw_(Instruction instruction);		// Loads 32bits word
		void exec_sh_(Instruction instruction);		// Store half word
		void exec_jal_(Instruction instruction);		// Jumpo and link
		void exec_sb_(Instruction instruction);		// Store byte
		void exec_andi_(Instruction instruction);		// Bitwise And Immediate
		void exec_lb_(Instruction instruction);		// Load byte
		void exec_beq_(Instruction instruction);		// Branch if equal
		void exec_bgtz_(Instruction instruction);		// Branch if grather than zero
		void exec_blez_(Instruction instruction);		// Branch if less than or equal to zero
		void exec_lbu_(Instruction instruction);		// Load Byte unsigned
		void exec_bxx_(Instruction instruction);		// Branch if xx (BLTZ, BLTZAL, BGEZ, BGEZAL)
		void exec_slti_(Instruction instruction);		// Set if less than immediate
		void exec_sltiu_(Instruction instruction);		// Set on Less Than Immediate Unsigned

		void exec_spec_(Instruction instruction);		// Special function
		void exec_sll_(Instruction instruction);		// Shift left logical
		void exec_or_(Instruction instruction);		// Shift left logical
		void exec_sltu_(Instruction instruction);		// Set on Less Than Unsigned
		void exec_addu_(Instruction instruction);		// Add Unsigned
		void exec_jr_(Instruction instruction);		// Jump Register
		void exec_and_(Instruction instruction);		// Bitwise And
		void exec_add_(Instruction instruction);		// Add
		void exec_jalr_(Instruction instruction);		// Jump and Link register
		void exec_subu_(Instruction instruction);		// Subtract unsigned
		void exec_sra_(Instruction instruction);		// Shift Right Arithmetic
		void exec_div_(Instruction instruction);		// Divide
		void exec_mflo_(Instruction instruction);		// Move From Lo
		void exec_srl_(Instruction instruction);		// Shift right logical
		void exec_divu_(Instruction instruction);		// Divide Unsigned
		void exec_mfhi_(Instruction instruction);		// Move From Hi
		void exec_slt_(Instruction instruction);		// Set on Less Than
		
		void exec_cop0_(Instruction instruction);	// Instruction for the coprocessor 0
		void exec_mtc0_(Instruction instruction);	//  Move to Coprocessor 0
		void exec_mfc0_(Instruction instruction);	//  Move from Coprocessor 0

	public:
		Core(Interconnect interconnect);
		void run_next_instruction();
		void set_reg(RegisterIdx reg_idx, u32 value);
		u32 get_reg(RegisterIdx reg_idx) const;
	};
}
