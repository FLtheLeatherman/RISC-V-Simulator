#ifndef ALU_HPP
#define ALU_HPP

#include <cstdint>

enum CalcType {
    kAdd, kSub, kShiftL, kShiftR, kAnd, kOr, kXor, kEqual, kNotEqual, kLess, kLessEqual, kGreater, kGreaterEqual, kDefault,
};

class ALU {
private:
    bool busy;
public:
    bool is_busy();
    void tick();
    uint32_t run(CalcType, uint32_t, uint32_t);
};

#endif // ALU_HPP