#include "ALU.hpp"

void ALU::init() {
    busy = false;
}
bool ALU::available() {
    return !busy;
}
void ALU::tick() {
    busy = false;
}
uint32_t ALU::run(CalcType type, uint32_t val1, uint32_t val2) {
    uint32_t res;
    busy = true;
    switch (type){
        case kAdd:
            res = val1 + val2;
            break;
        case kSub:
            res = val1 - val2;
            break;
        case kShiftL:
            res = val1 << val2;
            break;
        case kShiftR:
            res = val1 >> val2;
            break;
        case kShiftRArith:
            res = int(val1) >> val2;
        case kAnd:
            res = val1 & val2;
            break;
        case kOr:
            res = val1 | val2;
            break;
        case kXor:
            res = val1 ^ val2;
            break;
        case kEqual:
            res = val1 == val2;
            break;
        case kNotEqual:
            res = val1 != val2;
            break;
        case kLess:
            res = (int)val1 < (int)val2;
            break;
        case kLessUnsigned:
            res = val1 < val2;
            break;
        case kGreaterEqual:
            res = (int)val1 >= (int)val2;
            break;
        case kGreaterEqualUnsigned:
            res = val1 >= val2;
            break;
        default:
            break;
    }
    return res;
}