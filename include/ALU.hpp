#ifndef ALU_HPP
#define ALU_HPP

#include "Utility.hpp"
#include "ROB.hpp"

enum CalcType {
    kAdd, kSub, kShiftL, kShiftR, kAnd, kOr, kXor, kEqual, kNotEqual, kLess, kLessEqual, kGreater, kGreaterEqual, kDefault,
};

class ALU {
private:
    bool busy;
    ReorderBuffer *rob;
public:
    bool available();
    void tick();
    void run(CalcType, uint32_t, uint32_t, int); // 运算并广播
};

#endif // ALU_HPP