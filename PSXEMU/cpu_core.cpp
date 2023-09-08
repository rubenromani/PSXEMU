#include "cpu_core.h"

namespace CPU
{
	u32 Core::load32_(u32 address)
	{
		return interconnect_.load32(address);
	}

	void Core::decode_and_execute_(Instruction instruction)
	{
		//std::cout << std::hex << instruction.value <<std::endl;
		switch (instruction.function())
		{
			case ins_lui_:
			{
				exec_lui_(instruction);
			}break;
			default:
			{
				std::cerr << "Unhandled command" << std::endl;
				throw - 1;
			}break;
		}
	}

	void Core::exec_lui_(Instruction instruction)
	{
		u16 i = instruction.immediate();
		u8 t = instruction.target_register();

		u32 v = i << 16;
		set_reg(t, v);
	}


	Core::Core(Interconnect interconnect) :
		interconnect_(interconnect)
	{
		state_.pc = CPU_RESET_ADDRESS;
		for (int i = 0; i < N_GP_REG; i++)
		{
			state_.regs[i] = 0xdeadbeef;
		}
		state_.regs[0] = 0;
	}

	void Core::run_next_instruction()
	{
		u32 instruction = load32_(state_.pc);
		state_.pc += INSTR_LENGTH;
		decode_and_execute_(instruction);
	}

	void Core::set_reg(RegIdx reg_idx, u32 value)
	{
		state_.regs[reg_idx] = value;
	}

	u32 Core::get_reg(RegIdx reg_idx) const
	{
		return state_.regs[reg_idx];
	}

	void Core::set_reg(u8 reg_idx, u32 value)
	{
		state_.regs[reg_idx] = value;
	}

	u32 Core::get_reg(u8 reg_idx) const
	{
		return state_.regs[reg_idx];
	}
}