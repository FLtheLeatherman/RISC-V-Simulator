#include "RS.hpp"

ReservationStation::ReservationStation() {
    for (int i = 0; i < STATION_SIZE; ++i) {
        info[i].busy = false;
    }
}
bool ReservationStation::available() {
    bool res = false;
    for (int i = 0; i < STATION_SIZE; ++i) {
        if (!info[i].busy) {
            res = true;
            break;
        }
    }
    return res;
}
void ReservationStation::insert(CalcType calc_type, DataType data_type, uint32_t val1, uint32_t val2, uint32_t val3, uint32_t dest, int robEntry) {
    int pos = -1;
    for (int i = 0; i < STATION_SIZE; ++i) {
        if (!info[i].busy) {
            pos = i;
            break;
        }
    }
    info[pos].robEntry = robEntry;
    info[pos].type = calc_type;
    info[pos].busy = true;
    info[pos].ready = false;
    info[pos].dest = dest;
    int Qj, Qk;
    switch (data_type) {
        case kTwoReg: // val1, val2 是两个寄存器
            Qj = rf->get_tag(val1), Qk = rf->get_tag(val2);
            info[pos].Qj = Qj;
            if (Qj == -1) {
                info[pos].Vj = rf->get_val(val1);
            }
            info[pos].Qk = Qk;
            if (Qk == -1) {
                info[pos].Vk = rf->get_val(val2);
            }
            break;
        case kRegImm:
            Qj = rf->get_tag(val1);
            info[pos].Qj = Qj;
            if (Qj == -1) {
                info[pos].Vj = rf->get_val(val1);
            }
            info[pos].Vk = val2;
            info[pos].Qk = -1;
            break;
        case kTwoRegImm:
            Qj = rf->get_tag(val1), Qk = rf->get_tag(val2);
            info[pos].Qj = Qj;
            if (Qj == -1) {
                info[pos].Vj = rf->get_val(val1);
            }
            info[pos].Qk = Qk;
            if (Qk == -1) {
                info[pos].Vk = rf->get_val(val2);
            }
            info[pos].A = val3;
            break;
        case kImm:    
            info[pos].Qj = -1;
            info[pos].Vj = val1;
            break;
        default:
            break;
    }
}
void ReservationStation::update(int robEntry, uint32_t val) {
    for (int i = 0; i < STATION_SIZE; ++i) {
        if (info[i].busy && !info[i].ready) {
            if (info[i].Qj == robEntry) {
                info[i].Vj = val;
                info[i].Qj = -1;
            }
            if (info[i].Qk == robEntry) {
                info[i].Vk = val;
                info[i].Qk = -1;
            }
            if (info[i].Qj == -1 && info[i].Qk == -1) {
                info[i].ready = true;
            }
        }
    }
}
void ReservationStation::run() {
    for (int i = 0; i < STATION_SIZE; ++i) {
        if (info[i].busy && info[i].ready) {
            if (alu->available()) {
                alu->run(info[i].type, info[i].Vj, info[i].Vk, info[i].robEntry);
                break;
            }
        }
    }
}
void ReservationStation::tick() {
    for (int i = 0; i < STATION_SIZE; ++i) {
        info[i].robEntry.tick();
        info[i].type.tick();
        info[i].busy.tick();
        info[i].ready.tick();
        info[i].dest.tick();
        info[i].Vj.tick();
        info[i].Vk.tick();
        info[i].A.tick();
        info[i].Qj.tick();
        info[i].Qk.tick();
    }
}