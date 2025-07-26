#include "ALU.hpp"

bool ALU::available() {
    return !busy;
}
void ALU::tick() {
    busy = false;
}
void ALU::run(CalcType type, uint32_t val1, uint32_t val2, int robEntry) {
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
            res = val1 < val2;
            break;
        case kLessEqual:
            res = val1 <= val2;
            break;
        case kGreater:
            res = val1 > val2;
            break;
        case kGreaterEqual:
            res = val1 >= val2;
            break;
        default:
            break;
    }
    rob->update(robEntry, res);
    rs->update(robEntry, res);
    // @todo
    // call lsb to update next
}