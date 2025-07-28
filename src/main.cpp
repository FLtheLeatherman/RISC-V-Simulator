#include "Utility.hpp"
#include "Memory.hpp"
#include "CPU.hpp"
#include <iostream>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr), std::cout.tie(nullptr);
    Memory mem;
    mem.load();
    InstructionQueue iq;
    LoadStoreBuffer lsb;
    ReorderBuffer rob;
    RegisterFile rf;
    ALU alu;
    Predictor bp;
    ReservationStation rs;
    iq.init(&rs, &rob, &lsb, &bp, &mem);
    lsb.init();
    rob.init(&rs, &bp, &lsb, &iq, &rf);
    rf.init();
    alu.init();
    rs.init(&alu, &rf, &rob, &lsb, &mem);
    CPU cpu(&iq, &lsb, &rob, &rf, &alu, &bp, &rs);
    cpu.run();
}