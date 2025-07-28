#ifndef ALU_HPP
#define ALU_HPP

#include "Utility.hpp"
#include "ROB.hpp"
#include "RS.hpp"

class ALU {
private:
    bool busy;
public:
    ALU();
    bool available();
    void tick();
    uint32_t run(CalcType, uint32_t, uint32_t); // 运算并广播
    // wrong! 运算后的行为如何广播不定。
    // 由 RS 进行后续处理
};

#endif // ALU_HPP