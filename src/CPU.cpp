#include "CPU.hpp"

CPU::CPU(InstructionQueue *iq_, LoadStoreBuffer *lsb_, ReorderBuffer *rob_, RegisterFile *rf_, ALU *alu_, Predictor *bp_, ReservationStation *rs_, Memory *mem_) {
    iq = iq_;
    lsb = lsb_;
    rob = rob_;
    rf = rf_;
    alu = alu_;
    bp = bp_;
    rs = rs_;
    mem = mem_;
    clock = 0;
    halt = false;
}
void CPU::run() {
    while (!halt) {
        clock++;
        // std::cout << "clock: " << clock << '\n';
        // std::cout.flush();
        iq->run();
        // rob->print();
        // std::cout << "GOOD1" << std::endl;
        rs->run();
        // std::cout << "GOOD2" << std::endl;
        lsb->run();
        // std::cout << "GOOD3" << std::endl;
        rob->run();
        // rob->print();
        // std::cout << "GOOD4" << std::endl;
        rf->run();
        
        

        alu->tick();

        iq->tick();
        lsb->tick();
        rob->tick();
        // rob->print();
        rf->tick();
        rs->tick();

        if (rob->is_halt()) {
            halt = true;
        }
        // rf->print();
        // rob->print();
        // rs->print();
        // std::cout.flush();
        // mem->test();
        // if (clock >= 100) exit(0);
    }
}