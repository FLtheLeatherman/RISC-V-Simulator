#ifndef ROB_HPP
#define ROB_HPP

#include "Decoder.hpp"
#include "RegisterFile.hpp"
#include "Memory.hpp"

enum RoBType {
    kReg, kLoadByte, kLoadHalf, kLoadWord,
};

class ReorderBuffer {
private:
    struct RoBEntry {
        Register<bool> ready;
        Register<bool> busy;
        Register<RoBType> type;
        Register<uint32_t> val;
        Register<uint32_t> dest;
    };
    static constexpr int BUFFER_SIZE = 16;
    int head, tail;
    RoBEntry cir_que[BUFFER_SIZE];
    RegisterFile *RF;
    Memory *Mem;
public:
    ReorderBuffer();
    void tick(); // 检查队头能否 commit 即可
    bool available(); // 目前能否插入
    void insert(Instruction); // 加入一条指令
    void update(int, uint32_t); // 更新某条指令的状态
    void commit(); // 把值 commit 一下
    void run();
};

#endif // ROB_HPP