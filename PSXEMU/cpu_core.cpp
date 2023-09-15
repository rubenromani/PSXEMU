#include "cpu_core.h"

namespace CPU
{
	u32 Core::load32_(u32 address)
	{
		return interconnect_.load32(address);
	}

	u32 Core::store32_(u32 address, u32 value)
	{
		return interconnect_.store32(address, value);
	}

	void Core::decode_and_execute_(Instruction instruction)
	{
		std::cout << std::hex << instruction.value << std::endl;
		u32 ins = instruction.function();
		switch (ins)
		{
			case ins_lui_:
				exec_lui_(instruction);
				break;
			case ins_ori_:
				exec_ori_(instruction);
				break;
			case ins_sw_:
				exec_sw_(instruction);
				break;
			case ins_addiu_:
				exec_addiu_(instruction);
				break;
			case ins_j_:
				exec_j_(instruction);
				break;
			case ins_spec_:
				exec_spec_(instruction);
				break;
			default:
			{
				std::cerr << "Unhandled command" << std::endl;
				throw - 1;
			}break;
		}
	}

	void Core::exec_lui_(Instruction instruction)
	{
		auto i = instruction.immediate();
		auto t = instruction.target_register();
		auto v = i << 16;
		set_reg(t, v);
	}

	void Core::exec_ori_(Instruction instruction)
	{
		auto i = instruction.immediate();
		auto t = instruction.target_register();
		auto s = instruction.source_register();

		auto v = get_reg(s) | i;

		set_reg(t, v);
	}

	void Core::exec_sw_(Instruction instruction)
	{
		auto i = instruction.signed_immediate();
		auto t = instruction.target_register();
		auto s = instruction.source_register();

		auto addr = get_reg(s) + i;
		auto v = get_reg(t);

		store32_(addr, v);
	}

	void Core::exec_addiu_(Instruction instruction)
	{
		auto i = instruction.signed_immediate();
		auto t = instruction.target_register();
		auto s = instruction.source_register();

		auto v = get_reg(s) + i;

		set_reg(t, v);
	}

	void Core::exec_j_(Instruction instruction)
	{
		auto i = instruction.imm_jump();

		state_.pc = (state_.pc & 0xf0000000) | (i << 2);
	}

	void Core::exec_spec_(Instruction instruction)
	{
		auto subfunc = instruction.subfuction();
		switch (subfunc)
		{
		case ins_sll_:
			exec_sll_(instruction);
			break;
		case ins_or_:
			exec_or_(instruction);
			break;
		default:
			std::cerr << "Unhandled subfunction: " <<
				std::hex << subfunc <<std::endl;
			throw - 1;
			break;
		}
		
	}

	void Core::exec_sll_(Instruction instruction)
	{
		auto i = instruction.shift();
		auto t = instruction.target_register();
		auto d = instruction.d();

		auto v = get_reg(t) << i;

		set_reg(d, v);
	}

	void Core::exec_or_(Instruction instruction)
	{
		auto d = instruction.d();
		auto s = instruction.source_register();
		auto t = instruction.target_register();

		auto v = get_reg(s) | get_reg(t);

		set_reg(d, v);
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
		auto pc = state_.pc;
		Instruction instruction = next_instruction_;
		next_instruction_ = Instruction(load32_(state_.pc));
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