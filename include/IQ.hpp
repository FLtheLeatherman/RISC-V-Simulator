#ifndef IQ_HPP
#define IQ_HPP

#include "Utility.hpp"
#include "Decoder.hpp"
#include "Register.hpp"
#include "Predictor.hpp"
#include "RS.hpp"
#include "ROB.hpp"
#include "LSB.hpp"

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
    Register<uint32_t> flush_pc;
    IQEntry cir_que[16];
    Predictor *predictor;
    ReservationStation *rs;
    ReorderBuffer *rob;
    LoadStoreBuffer *lsb;
public:
    InstructionQueue();
    int insertInst(uint32_t); // 尝试向队中插入一个新指令，返回下一步的 PC 增量
    void launchInst(); // 把已经在队中的指令发射
    void run();
    void tick();
    void update_pc(uint32_t);
    void set_flush();
};

#endif // IQ_HPP