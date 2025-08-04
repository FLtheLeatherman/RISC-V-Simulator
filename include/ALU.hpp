#ifndef ALU_HPP
#define ALU_HPP

#include "Utility.hpp"
#include "ROB.hpp"
#include "RS.hpp"

class ALU {
private:
    bool busy;
public:
    void init();
    bool available();
    void tick();
    uint32_t run(CalcType, uint32_t, uint32_t); // 运算并广播
};

#endif // ALU_HPP