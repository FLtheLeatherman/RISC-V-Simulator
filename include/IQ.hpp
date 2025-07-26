#ifndef IQ_HPP
#define IQ_HPP

#include "Utility.hpp"
#include "Decoder.hpp"
#include "Register.hpp"

class InstructionQueue {
private:
    struct IQEntry {
        Register<bool> busy;
        Register<uint32_t> inst;
    };
    static constexpr int QUE_SIZE = 16;
    int head, tail;
    IQEntry cir_queue[16];
public:
    InstructionQueue();
    bool insertInst(Instruction); // 尝试向队中插入一个新指令，返回 PC 是否需要变化
    void launchInst(); // 把已经在队中的指令发射
    void run();
    void tick();
};

#endif // IQ_HPP