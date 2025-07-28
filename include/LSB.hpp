#ifndef LSB_HPP
#define LSB_HPP

#include "Register.hpp"

class LoadStoreBuffer {
private:
    struct LSBEntry {
        Register<bool> ready;
        Register<bool> busy;
    };
    static constexpr int BUFFER_SIZE = 16;
    Register<int> head, tail;
    LSBEntry cir_que[BUFFER_SIZE];
    Register<bool> need_flush;
public:
    void init();
    bool available();
    int insert();
    void update(int);
    bool is_head(int);
    void tick();
    void set_flush();
    void flush();
    void run();
};

#endif