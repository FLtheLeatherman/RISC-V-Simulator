#include "IQ.hpp"

InstructionQueue::InstructionQueue() {
    head = tail = 0;
}
bool InstructionQueue::insertInst(Instruction inst) {
    
}
void InstructionQueue::launchInst() {
    
}
void InstructionQueue::run() {
    if (head == tail) return;
    launchInst();
}
void InstructionQueue::tick() {
    for (int i = 0; i < QUE_SIZE; ++i) {
        cir_queue[i].busy.tick();
        cir_queue[i].inst.tick();
    }
}