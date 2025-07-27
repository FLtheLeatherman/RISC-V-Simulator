#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <cstring>
#include <string>

enum CalcType {
    kAdd, kSub, kShiftL, kShiftR, kAnd, kOr, kXor, kEqual, kNotEqual, kLess, kLessEqual, kGreater, kGreaterEqual, kDefault,
};

class ReorderBuffer;
class ReservationStation;
class InstructionQueue;
class ALU;

#endif // UTILS_HPP