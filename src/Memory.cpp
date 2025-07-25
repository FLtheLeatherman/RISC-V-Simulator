#include "Memory.hpp"

Memory::Memory() {
    memset(mem, 0, sizeof(mem));
}
uint8_t Memory::read(uint32_t addr) {
    return mem[addr];
}
uint16_t Memory::read_half_word(uint32_t addr) {
    return mem[addr] | (mem[addr + 1] << 8);
}
uint32_t Memory::read_word(uint32_t addr) {
    return mem[addr] | (mem[addr + 1] << 8) | (mem[addr + 2] << 16) | (mem[addr + 3] << 32);
}
void Memory::write(uint32_t addr, uint8_t imm) {
    mem[addr] = imm;
}
void Memory::write_half_word(uint32_t addr, uint16_t imm) {
    mem[addr] = imm & 0xFF;
    mem[addr + 1] = imm & (0xFF << 8);
}
void Memory::write_word(uint32_t addr, uint32_t imm) {
    mem[addr] = imm & 0xFF;
    mem[addr + 1] = imm & (0xFF << 8);
    mem[addr + 2] = imm & (0xFF << 16);
    mem[addr + 3] = imm & (0xFF << 24);
}