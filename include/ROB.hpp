#ifndef ROB_HPP
#define ROB_HPP

class ReorderBuffer {
private:
    struct RoBEntry {
        bool ready;
        int val;
        int dest;
    };
    static constexpr int BUFFER_SIZE = 16;
    int head, tail;
    RoBEntry cir_que[BUFFER_SIZE];
public:
    ReorderBuffer();
    void tick(); // 检查队头能否 commit 即可
    bool available(); // 目前能否插入
    void insert(); // 加入一条指令
    void update(); // 更新某条指令的状态
    void commit(); // 把值 commit 一下
};

#endif // ROB_HPP