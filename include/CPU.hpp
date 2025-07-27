#ifndef CPU_HPP
#define CPU_HPP

class CPU {
private:
    uint32_t program_counter;
    uint32_t clock;
public:
    CPU();
    void run();
}

#endif CPU_HPP