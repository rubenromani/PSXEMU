#include "cpu_core.h"

namespace CPU
{
	u32 Core::load32_(u32 address)
	{
		return interconnect_.load32(address);
	}

	u8 Core::load8_(u32 address)
	{
		return interconnect_.load8(address);
	}

	void Core::store32_(u32 address, u32 value)
	{
		interconnect_.store32(address, value);
	}

	void Core::store16_(u32 address, u16 value)
	{
		interconnect_.store16(address, value);
	}

	void Core::store8_(u32 address, u8 value)
	{
		interconnect_.store8(address, value);
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
			case ins_lw_:
				exec_lw_(instruction);
				break;
			case ins_sh_:
				exec_sh_(instruction);
				break;
			case ins_jal_:
				exec_jal_(instruction);
				break;
			case ins_sb_:
				exec_sb_(instruction);
				break;
			case ins_andi_:
				exec_andi_(instruction);
				break;
			case ins_lb_:
				exec_lb_(instruction);
				break;
			case ins_beq_:
				exec_beq_(instruction);
				break;
			case ins_bgtz_:
				exec_bgtz_(instruction);
				break;
			case ins_blez_:
				exec_blez_(instruction);
				break;
			case ins_lbu_:
				exec_lbu_(instruction);
				break;
			case ins_bxx_:
				exec_bxx_(instruction);
				break;
			case ins_slti_:
				exec_slti_(instruction);
				break;
			case ins_sltiu_:
				exec_sltiu_(instruction);
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

		if ((state_.cop0regs.sr & 0x10000) != 0)
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

	void Core::exec_lw_(Instruction instruction)
	{
		if ((state_.cop0regs.sr & 0x10000) != 0)
		{
			std::cout << "Ignoring store while cache is isolated" << std::endl;
			return;
		}

		auto i = instruction.signed_immediate();
		auto t = instruction.t();
		auto s = instruction.s();

		auto addr = get_reg(s) + i;

		auto v = load32_(addr);
		state_.load.first = t; 
		state_.load.second = v;
	}

	void Core::exec_sh_(Instruction instruction)
	{
		if ((state_.cop0regs.sr & 0x10000) != 0)
		{
			std::cout << "Ignoring store while cache is isolated" << std::endl;
			return;
		}

		auto i = instruction.signed_immediate();
		auto t = instruction.t();
		auto s = instruction.s();

		auto addr = get_reg(s) + i;
		u16 v = static_cast<u16>(get_reg(t));

		store16_(addr, v);

	}

	void Core::exec_jal_(Instruction instruction)
	{
		auto ra = state_.pc;

		set_reg(RegisterIdx(31), ra);

		exec_j_(instruction);
	}

	void Core::exec_sb_(Instruction instruction)
	{
		if ((state_.cop0regs.sr & 0x10000) != 0)
		{
			std::cout << "Ignoring store while cache is isolated" << std::endl;
			return;
		}

		auto i = instruction.signed_immediate();
		auto t = instruction.t();
		auto s = instruction.s();

		auto addr = get_reg(s) + i;
		u8 v = static_cast<u8>(get_reg(t));

		store8_(addr, v);
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
		case ins_sltu_:
			exec_sltu_(instruction);
			break;
		case ins_addu_:
			exec_addu_(instruction);
			break;
		case ins_jr_:
			exec_jr_(instruction);
			break;
		case ins_and_:
			exec_and_(instruction);
			break;
		case ins_add_:
			exec_add_(instruction);
			break;
		case ins_jalr_:
			exec_jalr_(instruction);
			break;
		case ins_subu_:
			exec_subu_(instruction);
			break;
		case ins_sra_:
			exec_sra_(instruction);
			break;
		case ins_div_:
			exec_div_(instruction);
			break;
		case ins_mflo_:
			exec_mflo_(instruction);
			break;
		case ins_srl_:
			exec_srl_(instruction);
			break;
		case ins_divu_:
			exec_divu_(instruction);
			break;
		case ins_mfhi_:
			exec_mfhi_(instruction);
			break;
		case ins_slt_:
			exec_slt_(instruction);
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

	void Core::exec_sltu_(Instruction instruction)
	{
		auto d = instruction.d();
		auto s = instruction.s();
		auto t = instruction.t();

		auto v = get_reg(s) < get_reg(t);

		set_reg(d, static_cast<u32>(v));
	}

	void Core::exec_addu_(Instruction instruction)
	{
		auto d = instruction.d();
		auto s = instruction.s();
		auto t = instruction.t();

		auto v = get_reg(s) + get_reg(t);

		set_reg(d, v);
	}

	void Core::exec_andi_(Instruction instruction)
	{
		auto i = instruction.immediate();
		auto t = instruction.t();
		auto s = instruction.s();

		auto v = get_reg(s) & i;

		set_reg(t, v);
	}

	void Core::exec_lb_(Instruction instruction)
	{
		auto i = instruction.signed_immediate();
		auto t = instruction.t();
		auto s = instruction.s();

		auto addr = get_reg(s) + i;

		s32 v = static_cast<s8>(load8_(addr));

		state_.load.first = t;
		state_.load.second = static_cast<u32>(v);
	}

	void Core::exec_beq_(Instruction instruction)
	{
		auto i = instruction.signed_immediate();
		auto s = instruction.s();
		auto t = instruction.t();

		if (get_reg(s) == get_reg(t))
		{
			branch(i);
		}
	}

	void Core::exec_bgtz_(Instruction instruction)
	{
		auto i = instruction.signed_immediate();
		auto s = instruction.s();

		s32 v = static_cast<s32>(get_reg(s));

		if (v > 0)
		{
			branch(i);
		}
	}

	void Core::exec_blez_(Instruction instruction)
	{
		auto i = instruction.signed_immediate();
		auto s = instruction.s();

		s32 v = static_cast<s32>(get_reg(s));

		if (v <= 0)
		{
			branch(i);
		}
	}

	void Core::exec_lbu_(Instruction instruction)
	{
		auto i = instruction.signed_immediate();
		auto t = instruction.t();
		auto s = instruction.s();

		auto addr = get_reg(s) + i;

		u32 v = static_cast<u32>(load8_(addr)) & 0x000000ff;

		state_.load.first = t;
		state_.load.second = v;
	}

	void Core::exec_bxx_(Instruction instruction)
	{
		auto i = instruction.signed_immediate();
		auto s = instruction.s();

		bool is_bgez = (instruction.value >> 16) & 0x1;
		bool is_link = (instruction.value >> 20) & 0x1 != 0;

		auto v = get_reg(s);

		u32 test = static_cast<u32>(v < 0);

		test = test ^ static_cast<u32>(is_bgez);

		if (test != 0)
		{
			if (is_link)
			{
				auto ra = state_.pc;

				set_reg(RegisterIdx(31), ra);
			}

			branch(i);
		}
	}

	void Core::exec_slti_(Instruction instruction)
	{
		s32 i = static_cast<s32>(instruction.signed_immediate());
		auto s = instruction.s();
		auto t = instruction.t();

		u32 v = static_cast<u32>(
			static_cast<s32>(get_reg(s)) < i);

		set_reg(t, v);
	}

	void Core::exec_sltiu_(Instruction instruction)
	{
		auto i = instruction.signed_immediate();
		auto s = instruction.s();
		auto t = instruction.t();

		u32 v = static_cast<u32>(get_reg(s) < i);

		set_reg(t, v);
	}

	void Core::exec_jr_(Instruction instruction)
	{
		auto s = instruction.s();

		state_.pc = get_reg(s);
	}

	void Core::exec_and_(Instruction instruction)
	{
		auto d = instruction.d();
		auto s = instruction.s();
		auto t = instruction.t();

		auto v = get_reg(s) & get_reg(t);

		set_reg(d, v);
	}

	void Core::exec_add_(Instruction instruction)
	{
		auto d = instruction.d();
		auto s = instruction.s();
		auto t = instruction.t();

		auto ss = static_cast<s32>(get_reg(s));
		auto tt = static_cast<s32>(get_reg(t));

		if ((tt > 0 && ss > INT_MAX - tt) || // `a + x` would overflow
			(tt < 0 && ss < INT_MIN - tt)) // `a + x` would underflow
		{
			std::cerr << "Overflow detected" << std::endl;
			throw - 1;
		}

		u32 v = static_cast<u32>(ss + tt);
		set_reg(d, v);

	}

	void Core::exec_jalr_(Instruction instruction)
	{
		auto d = instruction.d();
		auto s = instruction.s();

		auto ra = state_.pc;

		set_reg(d, ra);
		state_.pc = get_reg(s);
	}

	void Core::exec_subu_(Instruction instruction)
	{
		auto s = instruction.s();
		auto t = instruction.t();
		auto d = instruction.d();

		auto v = get_reg(s) + get_reg(t);

		set_reg(d, v);
	}

	void Core::exec_sra_(Instruction instruction)
	{
		auto i = instruction.shift();
		auto t = instruction.t();
		auto d = instruction.d();

		u32 v = static_cast<u32>(
			static_cast<s32>(get_reg(t)) >> i);

		set_reg(d, v);
	}

	void Core::exec_div_(Instruction instruction)
	{
		auto s = instruction.s();
		auto t = instruction.t();

		s32 n = static_cast<s32>(get_reg(s));
		s32 d = static_cast<s32>(get_reg(t));

		if (d == 0)
		{
			state_.hi = static_cast<u32>(n);
			if (n >= 0)
			{
				state_.lo = 0xffffffff;
			}
			else
			{
				state_.lo = 0x1;
			}
		}
		else if((static_cast<u32>(n) == 0x80000000) && 
				(d == -1))
		{
			state_.hi = 0x0;
			state_.lo = 0x80000000;
		}
		else
		{
			state_.hi = static_cast<u32>( n % d);
			state_.lo = static_cast<u32>(n / d);
		}
	}

	void Core::exec_mflo_(Instruction instruction)
	{
		auto d = instruction.d();
		
		set_reg(d, state_.lo);
	}

	void Core::exec_srl_(Instruction instruction)
	{
		auto i = instruction.shift();
		auto t = instruction.t();
		auto d = instruction.d();

		auto v = get_reg(t) >> i;
		set_reg(d, v);
	}

	void Core::exec_divu_(Instruction instruction)
	{
		auto s = instruction.s();
		auto t = instruction.t();

		auto n = get_reg(s);
		auto d = get_reg(t);

		if (d == 0)
		{
			state_.hi = n;
			state_.lo = 0xffffffff;
		}
		else
		{
			state_.hi = n % d;
			state_.lo = n / d;
		}
	}

	void Core::exec_mfhi_(Instruction instruction)
	{
		auto d = instruction.d();

		set_reg(d, state_.hi);
	}

	void Core::exec_slt_(Instruction instruction)
	{
		auto d = instruction.d();
		auto s = instruction.s();
		auto t = instruction.t();

		s32 ss = static_cast<s32>(get_reg(s));
		s32 tt = static_cast<s32>(get_reg(t));

		u32 v = static_cast<u32>(ss < tt);

		set_reg(d, v);
	}

	void Core::exec_cop0_(Instruction instruction)
	{
		switch (instruction.cop_opcode())
		{
		case ins_mtc0_:
			exec_mtc0_(instruction);
			break;
		case ins_mfc0_:
			exec_mfc0_(instruction);
			break;
		default:
			std::cerr << "Unhandled Coprocessor 0 instruction : " <<
				std::hex << instruction.cop_opcode() << std::endl;
			throw - 1;
		}
	}

	void Core::exec_mtc0_(Instruction instruction)
	{
		auto cpu_r = instruction.t();
		auto cop_r = instruction.d().value;

		auto v = get_reg(cpu_r);

		switch (cop_r)
		{
		case 3:
		case 5:
		case 6:
		case 7:
		case 9:
		case 11:
			if (v != 0)
			{
				std::cout << "Coprocessors Breakpoint registers " << cop_r <<
					": Unhandled write" << std::endl;
				throw - 1;
			}
			break;
		case 12:
			state_.cop0regs.sr = v;
			break;
		case 13:
			if (v != 0)
			{
				std::cout << "Coprocessors CAUSE registers " << cop_r <<
					": Unhandled write" << std::endl;
				throw - 1;
			}
			break;
		default:
			std::cout << "Unhandled Coprocessor Register : " << v << std::endl;
			throw - 1;
		}
	}

	void Core::exec_mfc0_(Instruction instruction)
	{
		auto cpu_r = instruction.t();
		auto cop_r = instruction.d().value;

		u32 v;

		switch (cop_r)
		{
		case 12:
			v = state_.cop0regs.sr;
			break;
		case 13:
			std::cout << "Coprocessors CAUSE registers " << cop_r <<
				": Unhandled read" << std::endl;
			throw - 1;
		default:
			std::cout << "Unhandled Read From Coprocessor 0 Register" << std::endl;
			throw - 1;
		}

		state_.load.first = cpu_r;
		state_.load.second = v;

	}


	Core::Core(Interconnect interconnect) :
		interconnect_(interconnect)
	{
		state_.pc = CPU_RESET_ADDRESS;
		state_.hi = 0xdeadbeef;
		state_.lo = 0xdeadbeef;
		for (int i = 0; i < N_GP_REG; i++)
		{
			state_.regs[i] = 0xdeadbeef;
			state_.out_regs[i] = 0xdeadbeef;
		}
		state_.regs[0] = 0;
		state_.out_regs[0] = 0;

		state_.load.first = RegisterIdx(0);
		state_.load.second = 0;

		state_.cop0regs.sr = 0;
	}

	void Core::copy_regs()
	{
		for (u8 i = 0; i < N_GP_REG; i++)
		{
			state_.regs[i] = state_.out_regs[i];
		}
	}

	void Core::run_next_instruction()
	{
		auto pc = state_.pc;
		Instruction instruction = next_instruction_;
		next_instruction_ = Instruction(load32_(state_.pc));
		state_.pc += INSTR_LENGTH;
		set_reg(state_.load.first, state_.load.second);
		state_.load.first = RegisterIdx(0);
		state_.load.second = 0;
		decode_and_execute_(instruction);
		copy_regs();
	}

	void Core::set_reg(RegisterIdx reg_idx, u32 value)
	{
		state_.out_regs[reg_idx.value] = value;
		state_.out_regs[0] = 0;
	}

	u32 Core::get_reg(RegisterIdx reg_idx) const
	{
		return state_.regs[reg_idx.value];
	}
}