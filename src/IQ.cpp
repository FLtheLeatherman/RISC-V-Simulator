#include "IQ.hpp"

void InstructionQueue::init(ReservationStation* rs_, ReorderBuffer* rob_, LoadStoreBuffer* lsb_, Predictor* predictor_, Memory* mem_) {
    rs = rs_;
    rob = rob_;
    lsb = lsb_;
    predictor = predictor_;
    mem = mem_;
    head = 0;
    tail = 0;
    pc = 0;
    flush_pc = 0;
    need_flush = 0;
    for (int i = 0; i < QUE_SIZE; ++i) {
        cir_que[i].busy = false;
    }
}
void InstructionQueue::insert_inst(uint32_t inst) {
    if (cir_que[tail].busy) return;
    cir_que[tail].busy = true;
    cir_que[tail].inst = inst;
    cir_que[tail].pc = pc;
    Instruction decoded_inst;
    decoded_inst.parse(inst);
    if (decoded_inst.op == Opcode::kJAL) {
        int tmp;
        // std::cout << decoded_inst.imm << std::endl;
        if (decoded_inst.imm & 0x80000000) {
            tmp = (int)(decoded_inst.imm - 0x100000000);
        } else {
            tmp = (int)decoded_inst.imm;
        }
        // std::cout << tmp << std::endl;
        pc = pc + tmp;
    } else if ((inst & 0x7F) == 0x63) {
        bool jump_flag = predictor->predict();
        cir_que[tail].jump = jump_flag;
        if (jump_flag) {
            pc = pc + decoded_inst.imm;
        } else {
            pc = pc + 4;
        }
    } else {
        pc = pc + 4;
    }
    tail = (tail + 1) % QUE_SIZE;
}
void InstructionQueue::launch_inst() {
    if (!cir_que[head].busy) return;
    if (!rs->available() || !rob->available()) return;
    // std::cout << "launch: " << std::hex << cir_que[head].inst << std::dec << std::endl;
    Instruction decoded_inst;
    decoded_inst.parse(cir_que[head].inst);
    // std::cout << decoded_inst.imm << '\n';
    uint32_t rob_entry;
    uint32_t cur_pc = cir_que[head].pc;
    if (cir_que[head].inst == 0x0FF00513) {
        rob_entry = rob->insert(RoBType::kHalt, 0, 0, cur_pc);
    } else if ((cir_que[head].inst & 0x7F) == 0x63) {
        rob_entry = rob->insert(RoBType::kBranch, cir_que[head].jump, cur_pc + decoded_inst.imm, cur_pc);
    } else if ((cir_que[head].inst & 0x7F) == 0x23) {
        if (!lsb->available()) {
            return;
        }
        uint32_t lsb_entry;
        if (decoded_inst.op == kSB) {
            lsb_entry = lsb->insert();
            rob_entry = rob->insert(RoBType::kStoreByte, lsb_entry, decoded_inst.rd, cur_pc);
        } else if (decoded_inst.op == kSH) {
            lsb_entry = lsb->insert();
            rob_entry = rob->insert(RoBType::kStoreHalf, lsb_entry, decoded_inst.rd, cur_pc);
        } else if (decoded_inst.op == kSW) {
            // std::cout << "store word!" << '\n';
            lsb_entry = lsb->insert();
            rob_entry = rob->insert(RoBType::kStoreWord, lsb_entry, decoded_inst.rd, cur_pc);
        }
    } else if ((cir_que[head].inst & 0x7F) == 0x03) {
        if (!lsb->available()) return;
        uint32_t lsb_entry;
        lsb_entry = lsb->insert();
        rob_entry = rob->insert(RoBType::kReg, lsb_entry, decoded_inst.rd, cur_pc);
    } else if (decoded_inst.op == Opcode::kJALR) {
        rob_entry = rob->insert(RoBType::kJalr, 0, decoded_inst.rd, cur_pc);
    } else if (decoded_inst.op == Opcode::kJAL) {
        rob_entry = rob->insert(RoBType::kReg, cur_pc + 4, decoded_inst.rd, cur_pc);
    } else {
        rob_entry = rob->insert(RoBType::kReg, 0, decoded_inst.rd, cur_pc);
    }
    // std::cout << decoded_inst.op << '\n';
    switch (decoded_inst.op) {
        case Opcode::kLUI:
            rob->update(rob_entry, decoded_inst.imm);
            break;
        case Opcode::kAUIPC:
            rob->update(rob_entry, cur_pc + decoded_inst.imm);
            break;
        case Opcode::kJAL:
            rob->update(rob_entry);
            break;
        case Opcode::kJALR:
            rs->insert(CalcType::kAdd, DataType::kRegImm, decoded_inst.rs1, decoded_inst.imm, 0, rob_entry);
            break;
        case Opcode::kBEQ:
            rs->insert(CalcType::kEqual, DataType::kTwoReg, decoded_inst.rs1, decoded_inst.rs2, 0, rob_entry);
            break;
        case Opcode::kBNE:
            rs->insert(CalcType::kNotEqual, DataType::kTwoReg, decoded_inst.rs1, decoded_inst.rs2, 0, rob_entry);
            break;
        case Opcode::kBLT:
            rs->insert(CalcType::kLess, DataType::kTwoReg, decoded_inst.rs1, decoded_inst.rs2, 0, rob_entry);
            break;
        case Opcode::kBGE:
            rs->insert(CalcType::kGreaterEqual, DataType::kTwoReg, decoded_inst.rs1, decoded_inst.rs2, 0, rob_entry);
            break;
        case Opcode::kBLTU:
            rs->insert(CalcType::kLessUnsigned, DataType::kTwoReg, decoded_inst.rs1, decoded_inst.rs2, 0, rob_entry);
            break;
        case Opcode::kBGEU:
            rs->insert(CalcType::kGreaterEqualUnsigned, DataType::kTwoReg, decoded_inst.rs1, decoded_inst.rs2, 0, rob_entry);
            break;
        case Opcode::kLB:
            rs->insert(CalcType::kLb, DataType::kRegImm, decoded_inst.rs1, decoded_inst.imm, 0, rob_entry);
            break;
        case Opcode::kLH:
            rs->insert(CalcType::kLh, DataType::kRegImm, decoded_inst.rs1, decoded_inst.imm, 0, rob_entry);
            break;
        case Opcode::kLW:
            rs->insert(CalcType::kLw, DataType::kRegImm, decoded_inst.rs1, decoded_inst.imm, 0, rob_entry);
            break;
        case Opcode::kLBU:
            rs->insert(CalcType::kLbu, DataType::kRegImm, decoded_inst.rs1, decoded_inst.imm, 0, rob_entry);
            break;
        case Opcode::kLHU:
            rs->insert(CalcType::kLhu, DataType::kRegImm, decoded_inst.rs1, decoded_inst.imm, 0, rob_entry);
            break;
        case Opcode::kSB:
            rs->insert(CalcType::kSb, DataType::kTwoRegImm, decoded_inst.rs2, decoded_inst.rs1, decoded_inst.imm, rob_entry);
            break;
        case Opcode::kSH:
            rs->insert(CalcType::kSh, DataType::kTwoRegImm, decoded_inst.rs2, decoded_inst.rs1, decoded_inst.imm, rob_entry);
            break;
        case Opcode::kSW:
            rs->insert(CalcType::kSw, DataType::kTwoRegImm, decoded_inst.rs2, decoded_inst.rs1, decoded_inst.imm, rob_entry);
            break;
        case Opcode::kADDI:
            // std::cout << "GOOD!\n";
            rs->insert(CalcType::kAdd, DataType::kRegImm, decoded_inst.rs1, decoded_inst.imm, 0, rob_entry);
            break;
        case Opcode::kSLTI:
            rs->insert(CalcType::kLess, DataType::kRegImm, decoded_inst.rs1, decoded_inst.imm, 0, rob_entry);
            break;
        case Opcode::kSLTIU:
            rs->insert(CalcType::kLessUnsigned, DataType::kRegImm, decoded_inst.rs1, decoded_inst.imm, 0, rob_entry);
            break;
        case Opcode::kXORI:
            rs->insert(CalcType::kXor, DataType::kRegImm, decoded_inst.rs1, decoded_inst.imm, 0, rob_entry);
            break;
        case Opcode::kORI:
            rs->insert(CalcType::kOr, DataType::kRegImm, decoded_inst.rs1, decoded_inst.imm, 0, rob_entry);
            break;
        case Opcode::kANDI:
            rs->insert(CalcType::kAnd, DataType::kRegImm, decoded_inst.rs1, decoded_inst.imm, 0, rob_entry);
            break;
        case Opcode::kSLLI:
            rs->insert(CalcType::kShiftL, DataType::kRegImm, decoded_inst.rs1, decoded_inst.imm, 0, rob_entry);
            break;
        case Opcode::kSRLI:
            rs->insert(CalcType::kShiftR, DataType::kRegImm, decoded_inst.rs1, decoded_inst.imm, 0, rob_entry);
            break;
        case Opcode::kSRAI:
            rs->insert(CalcType::kShiftRArith, DataType::kRegImm, decoded_inst.rs1, decoded_inst.imm, 0, rob_entry);
            break;
        case Opcode::kADD:
            rs->insert(CalcType::kAdd, DataType::kTwoReg, decoded_inst.rs1, decoded_inst.rs2, 0, rob_entry);
            break;
        case Opcode::kSUB:
            rs->insert(CalcType::kSub, DataType::kTwoReg, decoded_inst.rs1, decoded_inst.rs2, 0, rob_entry);
            break;
        case Opcode::kSLL:
            rs->insert(CalcType::kShiftL, DataType::kTwoReg, decoded_inst.rs1, decoded_inst.rs2, 0, rob_entry);
            break;
        case Opcode::kSLT:
            rs->insert(CalcType::kLess, DataType::kTwoReg, decoded_inst.rs1, decoded_inst.rs2, 0, rob_entry);
            break;
        case Opcode::kSLTU:
            rs->insert(CalcType::kLessUnsigned, DataType::kTwoReg, decoded_inst.rs1, decoded_inst.rs2, 0, rob_entry);
            break;
        case Opcode::kXOR:
            rs->insert(CalcType::kXor, DataType::kTwoReg, decoded_inst.rs1, decoded_inst.rs2, 0, rob_entry);
            break;
        case Opcode::kSRL:
            rs->insert(CalcType::kShiftR, DataType::kTwoReg, decoded_inst.rs1, decoded_inst.rs2, 0, rob_entry);
            break;
        case Opcode::kSRA:
            rs->insert(CalcType::kShiftRArith, DataType::kTwoReg, decoded_inst.rs1, decoded_inst.rs2, 0, rob_entry);
            break;
        case Opcode::kOR:
            rs->insert(CalcType::kOr, DataType::kTwoReg, decoded_inst.rs1, decoded_inst.rs2, 0, rob_entry);
            break;
        case Opcode::kAND:
            rs->insert(CalcType::kAnd, DataType::kTwoReg, decoded_inst.rs1, decoded_inst.rs2, 0, rob_entry);
            break;
        default:
            break;
    }
    cir_que[head].busy = false;
    head = (head + 1) % QUE_SIZE;
    // std::cout << "ALL GOOD\n";
}
void InstructionQueue::run() {
    if (need_flush) {
        flush();
        return;
    }
    launch_inst();
    uint32_t cur_inst = mem->read_word(pc);
    // std::cout << std::hex;
    // std::cout << pc << ": ";
    // std::cout << cur_inst << std::dec << std::endl;
    insert_inst(cur_inst);
}
void InstructionQueue::tick() {
    // if (need_flush) {
    //     flush();
    // }
    head.tick(), tail.tick();
    pc.tick();
    flush_pc.tick();
    need_flush.tick();
    for (int i = 0; i < QUE_SIZE; ++i) {
        cir_que[i].busy.tick();
        cir_que[i].inst.tick();
        cir_que[i].pc.tick();
        cir_que[i].jump.tick();
    }
    // if (need_flush) {
    //     return;
    // }
    // run();
}
void InstructionQueue::update_pc(uint32_t val) {
    flush_pc = val;
}
void InstructionQueue::set_flush() {
    need_flush = true;
}
void InstructionQueue::flush() {
    head = 0;
    tail = 0;
    need_flush = false;
    for (int i = 0; i < QUE_SIZE; ++i) {
        cir_que[i].busy = false;
    }
    pc = flush_pc;
}