#include "ROB.hpp"

ReorderBuffer::ReorderBuffer() {
    head = 0, tail = 0;
}
void ReorderBuffer::tick() {
    if (head == tail) return;
    if (cir_que[head].ready) {
        commit();
    }
}
bool ReorderBuffer::available() {
    return !cir_que[tail].busy;
}
void ReorderBuffer::insert(Instruction inst) {
    cir_que[tail].busy = true;
    cir_que[tail].ready = false;
    cir_que[tail].dest = inst.rd();
    tail = (tail + 1) % BUFFER_SIZE;
}
void ReorderBuffer::update(int robEntry, uint32_t val) {
    cir_que[robEntry].ready = true;
    cir_que[robEntry].val = val;
}
void ReorderBuffer::commit() {
    switch (cir_que[head].type) {
        case kReg:
            RF->set_val(cir_que[head].dest, cir_que[head].val);
            break;
        case kLoadByte:
            Mem->write(cir_que[head].dest, cir_que[head].val);
            break;
        case kLoadHalf:
            Mem->write_half_word(cir_que[head].dest, cir_que[head].val);
            break;
        case kLoadWord:
            Mem->write_word(cir_que[head].dest, cir_que[head].val);
            break;
    }
    cir_que[head].busy = false;
    cir_que[head].ready = false;
    head++;
}