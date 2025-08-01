#ifndef ROB_HPP
#define ROB_HPP

#include "Decoder.hpp"
#include "RegisterFile.hpp"
#include "Memory.hpp"
#include "IQ.hpp"
#include "LSB.hpp"
#include "Predictor.hpp"

enum RoBType {
    kReg, kStoreByte, kStoreHalf, kStoreWord, kHalt, kBranch, kJalr, kBranchSuccess, kBranchFail, kErr
};

class ReorderBuffer {
private:
    struct RoBEntry {
        Register<bool> ready;
        Register<bool> busy;
        Register<uint32_t> val;
        Register<uint32_t> dest;
        Register<uint32_t> pc;
        Register<int> lsb_entry;
        Register<RoBType> type;
    };
    static constexpr int BUFFER_SIZE = 16;
    Register<int> head, tail;
    RoBEntry cir_que[BUFFER_SIZE];
    RegisterFile *rf;
    ReservationStation *rs;
    InstructionQueue *iq;
    LoadStoreBuffer *lsb;
    Predictor *bp;
    Memory *mem;
    Register<bool> halt;
    Register<bool> need_flush;
public:
    void init(ReservationStation*, Predictor*, LoadStoreBuffer*, InstructionQueue*, RegisterFile*, Memory*);
    void tick(); // 检查队头能否 commit 即可
    bool available(); // 目前能否插入
    int insert(RoBType, uint32_t, uint32_t, uint32_t); // 加入一条指令
    void update(int); // 更新某条指令的状态
    void update(int, uint32_t, uint32_t);
    void update(int, uint32_t);
    void update_without_branch(int, uint32_t);
    void commit(); // 把值 commit 一下
    void run();
    void set_flush();
    void flush();
    void flush_all();
    uint32_t get_val(int);
    bool is_halt();
    void print();
};

#endif // ROB_HPP