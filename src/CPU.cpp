#include "CPU.hpp"

CPU::CPU(InstructionQueue *iq_, LoadStoreBuffer *lsb_, ReorderBuffer *rob_, RegisterFile *rf_, ALU *alu_, Predictor *bp_, ReservationStation *rs_) {
    iq = iq_;
    lsb = lsb_;
    rob = rob_;
    rf = rf_;
    alu = alu_;
    bp = bp_;
    rs = rs_;
    clock = 0;
    halt = false;
}
void CPU::run() {
    while (!halt) {
        clock++;
        iq->run();
        rs->run();
        lsb->run();
        rob->run();
        rf->run();
        
        alu->tick();

        iq->tick();
        lsb->tick();
        rob->tick();
        rf->tick();
        rs->tick();

        if (rob->is_halt()) {
            halt = true;
        }
    }
}