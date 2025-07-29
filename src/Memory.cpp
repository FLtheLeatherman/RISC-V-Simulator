#include "Memory.hpp"

void Memory::load() {
    std::string str;
    uint32_t addr;
    while (getline(std::cin, str)) {
        if (str[0] == '@') {
            addr = std::stoi(str.substr(1), nullptr, 16);
        } else {
            for (int i = 0; i < str.size(); i += 3) {
                write(addr, std::stoi(str.substr(i, 2), nullptr, 16));
                addr ++;
            }
        }
    }
}
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
    return mem[addr] | (mem[addr + 1] << 8) | (mem[addr + 2] << 16) | (mem[addr + 3] << 24);
}
void Memory::write(uint32_t addr, uint8_t imm) {
    mem[addr] = imm;
}
void Memory::write_half_word(uint32_t addr, uint16_t imm) {
    mem[addr] = imm & 0xFF;
    mem[addr + 1] = (imm & (0xFF << 8)) >> 8;
}
void Memory::write_word(uint32_t addr, uint32_t imm) {
    // std::cout << "????" << addr << std::endl;
    // std::cout << sizeof(mem) << std::endl;
    // std::cout << &mem << std::endl;
    mem[addr] = imm & 0xFF;
    // std::cout << "??" << std::endl;
    mem[addr + 1] = (imm & (0xFF << 8)) >> 8;
    mem[addr + 2] = (imm & (0xFF << 16)) >> 16;
    mem[addr + 3] = (imm & (0xFF << 24)) >> 24;
}
void Memory::test() {
    std::cout << &mem << std::endl;
}