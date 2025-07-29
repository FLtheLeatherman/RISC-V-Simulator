#ifndef RS_HPP
#define RS_HPP

#include "Utility.hpp"
#include "ALU.hpp"
#include "Decoder.hpp"
#include "RegisterFile.hpp"
#include "ROB.hpp"
#include "Memory.hpp"

enum DataType {
    kTwoReg, kRegImm, kTwoRegImm, kImm,
};

class ReservationStation {
private:
    struct RSEntry {
        Register<int> rob_entry;
        Register<CalcType> calc_type;
        Register<DataType> data_type;
        Register<bool> busy;
        Register<uint32_t> Vj, Vk;
        Register<uint32_t> A; // Imm
        Register<int> Qj, Qk;
    };
    static constexpr int STATION_SIZE = 16;
    RSEntry info[STATION_SIZE];
    Register<bool> need_flush;
    ALU *alu;
    RegisterFile *rf;
    ReorderBuffer *rob;
    LoadStoreBuffer *lsb;
    Memory *mem;
public:
    void init(ALU*, RegisterFile*, ReorderBuffer*, LoadStoreBuffer*, Memory*);
    bool available();
    void insert(CalcType, DataType, uint32_t, uint32_t, uint32_t, int); // 加入一条指令
    void update(int, uint32_t); // 更新单个指令的值
    void run(); // 如果可以的话送到 ALU 进行计算.
    void tick();
    void set_flush();
    void flush();
    void print();
};

#endif // RS_HPP