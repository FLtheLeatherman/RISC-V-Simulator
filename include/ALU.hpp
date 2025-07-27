#ifndef ALU_HPP
#define ALU_HPP

#include "Utility.hpp"
#include "ROB.hpp"
#include "RS.hpp"

class ALU {
private:
    bool busy;
    ReorderBuffer *rob;
    ReservationStation *rs;
public:
    bool available();
    void tick();
    void run(CalcType, uint32_t, uint32_t, int); // 运算并广播
};

#endif // ALU_HPP