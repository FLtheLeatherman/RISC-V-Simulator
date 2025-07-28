#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <cstring>
#include <string>

enum CalcType {
    kAdd, kSub, kShiftL, kShiftR, kShiftRArith, kAnd, kOr, kXor, kEqual, kNotEqual, kLess, kLessUnsigned, kGreaterEqual, kGreaterEqualUnsigned, kLb, kLh, kLw, kLbu, kLhu, kSb, kSh, kSw, kDefault,
};

class RegisterFile;
class ReorderBuffer;
class ReservationStation;
class InstructionQueue;
class ALU;
class LoadStoreBuffer;
class Predictor;

#endif // UTILS_HPP