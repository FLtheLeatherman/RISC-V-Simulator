#include "Utility.hpp"
#include "Memory.hpp"
#include "CPU.hpp"
#include <iostream>

int main() {
    freopen("sample.data", "r", stdin);
    freopen("log.out", "w", stdout);
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr), std::cout.tie(nullptr);
    Memory mem;
    mem.load();
    std::cout << "load success" << '\n';
    std::cout.flush();
    InstructionQueue iq;
    LoadStoreBuffer lsb;
    ReorderBuffer rob;
    RegisterFile rf;
    ALU alu;
    Predictor bp;
    ReservationStation rs;
    iq.init(&rs, &rob, &lsb, &bp, &mem);
    lsb.init();
    rob.init(&rs, &bp, &lsb, &iq, &rf, &mem);
    rf.init();
    alu.init();
    rs.init(&alu, &rf, &rob, &lsb, &mem);
    // mem.test();
    CPU cpu(&iq, &lsb, &rob, &rf, &alu, &bp, &rs, &mem);
    cpu.run();
}