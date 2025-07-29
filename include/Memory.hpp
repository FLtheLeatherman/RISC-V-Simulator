#ifndef MEMORY_HPP
#define MEMORY_HPP

#include "Utility.hpp"
#include <iostream>

class Memory {
private:
    static constexpr int MEM_SIZE = 0x400000;
    uint8_t mem[MEM_SIZE];
public:
    Memory();
    void load();
    uint8_t read(uint32_t);
    uint16_t read_half_word(uint32_t);
    uint32_t read_word(uint32_t);
    void write(uint32_t, uint8_t);
    void write_half_word(uint32_t, uint16_t);
    void write_word(uint32_t, uint32_t);
    void test();
};

#endif