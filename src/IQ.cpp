#include "IQ.hpp"

InstructionQueue::InstructionQueue() {
    head = 0;
    tail = 0;
}
int InstructionQueue::insertInst(uint32_t inst) {
    if (cir_que[tail].busy) return 0;
    cir_que[tail].busy = true;
    cir_que[tail].inst = inst;
    Instruction decoded_inst;
    decoded_inst.parse(inst);
    int res;
    if (decoded_inst.op == Opcode::kJAL) {
        res = decoded_inst.imm;
    } else if ((inst & 0x7F) == 0x63) {
        bool jump_flag = predictor->predict();
        if (jump_flag) {
            res = decoded_inst.imm;
        } else {
            res = 4;
        }
    } else {
        res = 4;
    }
    tail = (tail + 1) % QUE_SIZE;
    return res;
}
void InstructionQueue::launchInst() {
    if (!cir_que[head].busy) return;
    if (!rs->available() || !rob->available()) return;
    Instruction decoded_inst;
    decoded_inst.parse(cir_que[head].inst);
    uint32_t robEntry;
    uint32_t cur_pc = cir_que[head].pc;
    if (cir_que[head].inst == 0x0FF00513) {
        robEntry = rob->insert(RoBType::kHalt, 0, 0, cur_pc);
    } else if ((cir_que[head].inst & 0x7F) == 0x63) {
        robEntry = rob->insert(RoBType::kBranch, cir_que[head].jump, cur_pc + decoded_inst.imm, cur_pc); // 需修改！！传入跳到哪里去
    } else {
        // @todo
        // 其它类型操作
    }
    cir_que[head].busy = false;
    head = (head + 1) % QUE_SIZE;
}
void InstructionQueue::run() {
    if (!cir_que[head].busy) return;
    launchInst();
}
void InstructionQueue::tick() {
    head.tick(), tail.tick();
    for (int i = 0; i < QUE_SIZE; ++i) {
        cir_que[i].busy.tick();
        cir_que[i].inst.tick();
    }
}
void InstructionQueue::update_pc(uint32_t val) {
    flush_pc = val;
}
void InstructionQueue::set_flush() {
    
}