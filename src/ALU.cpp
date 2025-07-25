#include "ALU.hpp"

bool ALU::is_busy() {
    return busy;
}
void ALU::tick() {
    busy = false;
}
uint32_t ALU::run(CalcType type, uint32_t val1, uint32_t val2) {
    busy = true;
    switch (type){
        case kAdd:
            return val1 + val2;
        case kSub:
            return val1 - val2;
        case kShiftL:
            return val1 << val2;
        case kShiftR:
            return val1 >> val2;
        case kAnd:
            return val1 & val2;
        case kOr:
            return val1 | val2;
        case kXor:
            return val1 ^ val2;
        case kEqual:
            return val1 == val2;
        case kNotEqual:
            return val1 != val2;
        case kLess:
            return val1 < val2;
        case kLessEqual:
            return val1 <= val2;
        case kGreater:
            return val1 > val2;
        case kGreaterEqual:
            return val1 >= val2;
        default:
            break;
    }
    return -114;
}