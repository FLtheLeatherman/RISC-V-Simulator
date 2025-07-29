#ifndef CPU_HPP
#define CPU_HPP

#include "Utility.hpp"
#include "IQ.hpp"
#include "LSB.hpp"
#include "ROB.hpp"
#include "RegisterFile.hpp"
#include "ALU.hpp"
#include "Predictor.hpp"
#include "RS.hpp"
#include "Memory.hpp"

class CPU {
private:
    uint32_t clock;
    bool halt;
    InstructionQueue *iq;
    LoadStoreBuffer *lsb;
    ReorderBuffer *rob;
    RegisterFile *rf;
    ALU *alu;
    Predictor *bp;
    ReservationStation *rs;
    Memory *mem;
public:
    CPU(InstructionQueue*, LoadStoreBuffer*, ReorderBuffer*, RegisterFile*, ALU*, Predictor*, ReservationStation*, Memory*);
    void run();
};

#endif // CPU_HPP