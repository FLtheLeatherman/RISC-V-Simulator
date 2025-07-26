#ifndef LSB_HPP
#define LSB_HPP

class LoadStoreBuffer {
private:
    struct LSBEntry {

    };
    static constexpr int BUFFER_SIZE = 16;
    
public:
    LoadStoreBuffer();
    bool available();
};

#endif