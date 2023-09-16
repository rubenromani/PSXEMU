#include "cpu_core.h"

namespace CPU
{
	u32 Core::load32_(u32 address)
	{
		return interconnect_.load32(address);
	}

	void Core::store32_(u32 address, u32 value)
	{
		interconnect_.store32(address, value);
	}

	void Core::branch(u32 offset)
	{
		auto shifted_offset = offset << 2;		// align on 32bits
		state_.pc = state_.pc + shifted_offset;
		state_.pc = state_.pc - INSTR_LENGTH;	// compensation for pipeline
	}

	void Core::decode_and_execute_(Instruction instruction)
	{
		std::cout << "Instruction: " << std::hex << instruction.value << 
			"\tPC: " << state_.pc << std::endl;
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
			case ins_addi_:
				exec_addi_(instruction);
				break;
			case ins_j_:
				exec_j_(instruction);
				break;
			case ins_bne_:
				exec_bne_(instruction);
				break;
			case ins_spec_:
				exec_spec_(instruction);
				break;
			case ins_cop0_:
				exec_cop0_(instruction);
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
		auto t = instruction.t();
		auto v = i << 16;
		set_reg(t, v);
	}

	void Core::exec_ori_(Instruction instruction)
	{
		auto i = instruction.immediate();
		auto t = instruction.t();
		auto s = instruction.s();

		auto v = get_reg(s) | i;

		set_reg(t, v);
	}

	void Core::exec_sw_(Instruction instruction)
	{

		if ((state_.sr & 0x10000) != 0)
		{
			std::cout << "Ignoring store while cache is isolated" << std::endl;
			return;
		}

		auto i = instruction.signed_immediate();
		auto t = instruction.t();
		auto s = instruction.s();

		auto addr = get_reg(s) + i;
		auto v = get_reg(t);

		store32_(addr, v);
	}

	void Core::exec_addiu_(Instruction instruction)
	{
		auto i = instruction.signed_immediate();
		auto t = instruction.t();
		auto s = instruction.s();

		auto v = get_reg(s) + i;

		set_reg(t, v);
	}

	void Core::exec_addi_(Instruction instruction)
	{
		s32 i = static_cast<s32>(instruction.signed_immediate());
		auto t = instruction.t();
		auto s = instruction.s();

		s32 ss = static_cast<s32>(get_reg(s));

		if ((i > 0 && ss > INT_MAX - i) || // `a + x` would overflow
			(i < 0 && ss < INT_MIN - i)) // `a + x` would underflow
		{
			std::cerr << "Overflow detected" << std::endl;
			throw - 1;
		}

		u32 v = static_cast<u32>(ss + i);

		set_reg(t, v);
	}

	void Core::exec_j_(Instruction instruction)
	{
		auto i = instruction.imm_jump();

		state_.pc = (state_.pc & 0xf0000000) | (i << 2);
	}

	void Core::exec_bne_(Instruction instruction)
	{
		auto i = instruction.signed_immediate();
		auto s = instruction.s();
		auto t = instruction.t();

		if (get_reg(s) != get_reg(t))
		{
			branch(i);
		}
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
		auto t = instruction.t();
		auto d = instruction.d();

		auto v = get_reg(t) << i;

		set_reg(d, v);
	}

	void Core::exec_or_(Instruction instruction)
	{
		auto d = instruction.d();
		auto s = instruction.s();
		auto t = instruction.t();

		auto v = get_reg(s) | get_reg(t);

		set_reg(d, v);
	}

	void Core::exec_cop0_(Instruction instruction)
	{
		switch (instruction.cop_opcode())
		{
		case ins_mtc0_:
			exec_mtc0_(instruction);
			break;
		default:
			break;
		}
	}

	void Core::exec_mtc0_(Instruction instruction)
	{
		auto cpu_r = instruction.t();
		auto cop_r = instruction.d().value;

		auto v = get_reg(cpu_r);

		switch (cop_r)
		{
		case 12:
			state_.sr = v;
			break;
		default:
			std::cout << "Unhandled Coprocessor Register : " << v << std::endl;
			throw - 1;
		}
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

		state_.sr = 0;
	}

	void Core::run_next_instruction()
	{
		auto pc = state_.pc;
		Instruction instruction = next_instruction_;
		next_instruction_ = Instruction(load32_(state_.pc));
		state_.pc += INSTR_LENGTH;
		decode_and_execute_(instruction);
	}

	void Core::set_reg(RegisterIdx reg_idx, u32 value)
	{
		state_.regs[reg_idx.value] = value;
	}

	u32 Core::get_reg(RegisterIdx reg_idx) const
	{
		return state_.regs[reg_idx.value];
	}
}