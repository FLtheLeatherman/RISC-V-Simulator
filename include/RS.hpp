#ifndef RS_HPP
#define RS_HPP

#include "ALU.hpp"
#include "Decoder.hpp"
#include "RegisterFile.hpp"
#include "ROB.hpp"

enum DataType {
    kTwoReg, kRegImm, kTwoRegImm, kImm,
};

class ReservationStation {
private:
    struct RSEntry {
        Register<int> robEntry;
        Register<CalcType> type;
        Register<bool> busy;
        Register<bool> ready;
        Register<uint32_t> dest;
        Register<uint32_t> Vj, Vk;
        Register<uint32_t> A; // Imm
        Register<int> Qj, Qk;
    };
    static constexpr int STATION_SIZE = 16;
    RSEntry info[STATION_SIZE];
    ALU *alu;
    RegisterFile *rf;
    ReorderBuffer *rob;
public:
    ReservationStation();
    bool available();
    void insert(CalcType, DataType, uint32_t, uint32_t, uint32_t, uint32_t, int); // 加入一条指令
    void update(int, uint32_t); // 更新单个指令的值
    void run(); // 如果可以的话送到 ALU 进行计算.
    void tick();
    void set_flush();
};

#endif // RS_HPP