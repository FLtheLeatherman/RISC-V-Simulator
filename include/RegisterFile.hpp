#ifndef REGISTER_FILE_HPP
#define REGISTER_FILE_HPP

#include "Register.hpp"

// 0 寄存器应当总为 0.
class RegisterFile {
private:
    static constexpr int REG_NUM = 32;
    Register<uint32_t> registers[REG_NUM];
    Register<int> tag[REG_NUM];
    Register<bool> busy[REG_NUM];
    Register<bool> need_flush;
    Register<bool> need_write[REG_NUM];
    Register<bool> need_clear[REG_NUM];
public:
    RegisterFile();
    void tick();
    void write_tag(uint32_t, int);
    int read_tag(uint32_t);
    uint32_t read_reg(uint32_t);
    void write_reg(uint32_t, uint32_t, int);
    void set_flush();
    void flush();
    bool readable(uint32_t, int);
    void run();
};

#endif // REGISTER_FILE_HPP