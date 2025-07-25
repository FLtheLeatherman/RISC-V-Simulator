#ifndef REGISTER_FILE_HPP
#define REGISTER_FILE_HPP

#include "Register.hpp"

// 0 寄存器应当总为 0.
class RegisterFile {
private:
    static constexpr int REG_NUM = 32;
    Register<uint32_t> registers[REG_NUM];
    int tag[REG_NUM];
public:
    RegisterFile();
    void tick();
    void set_stat(uint32_t, int);
    void set_val(uint32_t, uint32_t); // 给**下一轮**赋值
    bool is_busy(uint32_t);
    int get_tag(uint32_t);
    uint32_t get_val(uint32_t);
};

#endif // REGISTER_FILE_HPP