#ifndef DECODER_HPP
#define DECODER_HPP

#include "Utility.hpp"

enum Opcode {
    kLUI, kAUIPC, kJAL, kJALR, kBEQ, kBNE, kBLT, kBGE, kBLTU, kBGEU, kLB, kLH, kLW, kLBU, kLHU, kSB, kSH, kSW, kADDI, kSLTI, kSLTIU, kXORI, kORI, kANDI, kSLLI, kSRLI, kSRAI, kADD, kSUB, kSLL, kSLT, kSLTU, kXOR, kSRL, kSRA, kOR, kAND, kDEFAULT,
};

class Instruction {
private:
    Opcode op;
    uint32_t rd;
    uint32_t rs1, rs2;
    uint32_t imm;
    uint32_t shamt;
public:
    void parse(uint32_t);
};

#endif // DECODER_HPP