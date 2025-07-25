#include "Decoder.hpp"

void Instruction::Parse(uint32_t inst) {
    uint32_t op_num = inst & 0x7F;
    switch (op_num) {
        case 0x37: // LUI
            op = Opcode::kLUI;
            rd = (inst >> 7) & 0x1F;
            imm = inst & 0xFFFFF000;
            break;
        case 0x17: // AUIPC
            op = Opcode::kAUIPC;
            rd = (inst >> 7) & 0x1F;
            imm = inst & 0xFFFFF000;
            break;
        case 0x6F: // JAL
            op = Opcode::kJAL;
            rd = (inst >> 7) & 0x1F;
            imm = (inst & 0xFF000) // [19:12]
                | ((inst & 0x100000) >> 10) // [11]
                | ((inst & 0x7FE00000) >> 20) // [10:1]
                | ((inst & 0x80000000) >> 11); // [20]
            break;
        case 0x67: // JALR
            op = Opcode::kJALR;
            rd = (inst >> 7) & 0x1F;
            rs1 = (inst >> 15) & 0x1F;
            imm = inst >> 20;
            break;
        case 0x63: // B-type, from beq to bgeu
            uint32_t funct3 = (inst >> 12) & 0x7;
            rs1 = (inst >> 15) & 0x1F;
            rs2 = (inst >> 20) & 0x1F;
            imm = ((inst & 0x80) << 3) // [11]
                | ((inst & 0xF00) >> 8) // [4:1]
                | ((inst & 0x7E000000) >> 21) // [10:5]
                | ((inst & 0x80000000) >> 20); // [20]
            switch (funct3) {
                case 0x0: // beq
                    op = Opcode::kBEQ;
                    break;
                case 0x1: // bne
                    op = Opcode::kBNE;
                    break;
                case 0x4: // blt
                    op = Opcode::kBLT;
                    break;
                case 0x5: // bge
                    op = Opcode::kBGE;
                    break;
                case 0x6: // bltu
                    op = Opcode::kBLTU;
                    break;
                case 0x7: // bgeu
                    op = Opcode::kBGEU;
                    break;
                default:
                    break;
            }
            break;
        case 0x03: // I-type, from lb to lhu
            uint32_t funct3 = (inst >> 12) & 0x7;   
            rd = (inst >> 7) & 0x1F;
            rs1 = (inst >> 15) & 0x1F;
            imm = (inst >> 20) & 0xFFF;
            switch (funct3) {
                case 0x0: // lb
                    op = Opcode::kLB;
                    break;
                case 0x1: // lh
                    op = Opcode::kLH;
                    break;
                case 0x2: // lw
                    op = Opcode::kLW;
                    break;
                case 0x4: // lbu
                    op = Opcode::kLBU;
                    break;
                case 0x5: // lhu
                    op = Opcode::kLHU;
                default:
                    break;
            }
            break;
        case 0x32: // S-type, from sb to sh
            uint32_t funct3 = (inst >> 12) & 0x7;
            rs1 = (inst >> 15) & 0x1F;
            rs2 = (inst >> 20) & 0x1F;
            imm = (inst >> 7) & 0x1f
                | (inst >> 21) & 0x3F;
            switch (funct3) {
                case 0x0: // sb
                    op = Opcode::kSB;
                    break;
                case 0x1: // sh
                    op = Opcode::kSH;
                    break;
                case 0x2: // sw
                    op = Opcode::kSW;
                    break;
                default:
                    break;
                }
            break;
        case 0x31: // I-type, from addi to srai
            uint32_t funct3 = (inst >> 12) & 0x7;   
            rd = (inst >> 7) & 0x1F;
            rs1 = (inst >> 15) & 0x1F;
            imm = inst >> 20;
            switch (funct3) {
                case 0x0: // addi
                    op = Opcode::kADDI;
                    break;
                case 0x2: // slti
                    op = Opcode::kSLTI;
                    break;
                case 0x3: // sltiu
                    op = Opcode::kSLTIU;
                    break;
                case 0x4: // xori
                    op = Opcode::kXORI;
                    break;
                case 0x6: // ori
                    op = Opcode::kORI;
                    break;
                case 0x7: // andi
                    op = Opcode::kANDI;
                    break;
                case 0x1: // slli
                    op = Opcode::kSLLI;
                    shamt = (inst >> 20) & 0x1F;
                    break;
                case 0x5:
                    shamt = (inst >> 20) & 0x1F;
                    int32_t funct7 = inst >> 25;
                    if (funct7 == 0x0) { // srli
                        op = Opcode::kSRLI;
                    } else { // srai
                        op = Opcode::kSRAI;
                    }
                    break;
                default:
                    break;
            }
            break;
        case 0x33: // R-type, from add to and
            uint32_t funct3 = (inst >> 12) & 0x7;   
            uint32_t funct7 = inst >> 25;
            rd = (inst >> 7) & 0x1F;
            rs1 = (inst >> 15) & 0x1F;
            rs2 = (inst >> 20) & 0x1F;
            switch (funct3) {
                case 0x0:
                    if (!funct7) { // add
                        op = Opcode::kADD;
                    } else { // sub
                        op = Opcode::kSUB;
                    }
                    break;
                case 0x1: // sll
                    op = Opcode::kSLL;
                    break;
                case 0x2: // slt
                    op = Opcode::kSLT;
                    break;
                case 0x3: // sltu
                    op = Opcode::kSLTU;
                    break;
                case 0x4: // xor
                    op = Opcode::kXOR;
                    break;
                case 0x5:
                    if (!funct7) { // srl
                        op = Opcode::kSRL;
                    } else { // sra
                        op = Opcode::kSRA;
                    }
                    break;
                case 0x6: // or
                    op = Opcode::kOR;
                case 0x7: // and
                    op = Opcode::kAND;
                default:
                    break;
            }
        default:
            break;
    }
}