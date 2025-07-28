#ifndef IQ_HPP
#define IQ_HPP

#include "Utility.hpp"
#include "Decoder.hpp"
#include "Register.hpp"
#include "Predictor.hpp"
#include "RS.hpp"
#include "ROB.hpp"
#include "LSB.hpp"
#include "Memory.hpp"

class InstructionQueue {
private:
    struct IQEntry {
        Register<bool> busy;
        Register<bool> jump;
        Register<uint32_t> pc;
        Register<uint32_t> inst;
    };
    static constexpr int QUE_SIZE = 16;
    Register<int> head, tail;
    IQEntry cir_que[16];
    Register<uint32_t> pc;
    Register<bool> need_flush;
    Register<uint32_t> flush_pc;
    Predictor *predictor;
    ReservationStation *rs;
    ReorderBuffer *rob;
    LoadStoreBuffer *lsb;
    Memory *mem;
public:
    InstructionQueue(ReservationStation*, ReorderBuffer*, LoadStoreBuffer*, Predictor*, Memory*);
    void insert_inst(uint32_t); // 尝试向队中插入一个新指令
    void launch_inst(); // 把已经在队中的指令发射
    void run();
    void tick();
    void update_pc(uint32_t);
    void set_flush();
    void flush();
};

#endif // IQ_HPP