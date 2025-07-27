#ifndef LSB_HPP
#define LSB_HPP

#include "Register.hpp"

class LoadStoreBuffer {
private:
    struct LSBEntry {
        bool ready;
        bool busy;
    };
    static constexpr int BUFFER_SIZE = 16;
    Register<int> head, tail;
    LSBEntry cir_que[BUFFER_SIZE];
public:
    LoadStoreBuffer();
    bool available();
    void insert();
    void update(int);
    bool is_head(int);
    void tick();
};

#endif