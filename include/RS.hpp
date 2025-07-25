#ifndef RS_HPP
#define RS_HPP

#include "ALU.hpp"

class ReservationStation {
private:
    struct RSEntry {
        int robEntry;
        CalcType type;
        bool busy;
        uint32_t Dest;
        uint32_t Vj, Vk;
        int Qj, Qk;
    };
    static constexpr int STATION_SIZE = 16;
    RSEntry info[STATION_SIZE];
public:
    ReservationStation();
    bool available();
    void insert(); // 加入一条指令
    void update(); // 更新单个指令的值
    void tick(); // 如果可以的话送到 ALU 进行计算.
};

#endif // RS_HPP