#ifndef IQ_HPP
#define IQ_HPP

class InstructionQueue {
private:
    static constexpr int QUE_SIZE = 16;
    int head, tail;
    uint32_t inst[QUE_SIZE];
    bool is_jump[QUE_SIZE];
public:
    
};

#endif