#include "RS.hpp"

ReservationStation::ReservationStation(ALU *alu_, RegisterFile *rf_, ReorderBuffer *rob_, LoadStoreBuffer *lsb_) {
    alu = alu_;
    rf = rf_;
    rob = rob_;
    lsb = lsb_;
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
void ReservationStation::insert(CalcType calc_type, DataType data_type, uint32_t val1, uint32_t val2, uint32_t val3, int rob_entry) {
    int pos = -1;
    for (int i = 0; i < STATION_SIZE; ++i) {
        if (!info[i].busy) {
            pos = i;
            break;
        }
    }
    info[pos].rob_entry = rob_entry;
    info[pos].calc_type = calc_type;
    info[pos].data_type = data_type;
    info[pos].busy = true;
    info[pos].ready = false;
    if (data_type == DataType::kTwoReg) {
        if (rf->readable(val1, rob_entry)) {
            
        }
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
                alu->run(info[i].calc_type, info[i].Vj, info[i].Vk, info[i].rob_entry);
                break;
            }
        }
    }
}
void ReservationStation::tick() {
    if (need_flush) {
        flush();
    } else {
        run();
    }
    need_flush.tick();
    for (int i = 0; i < STATION_SIZE; ++i) {
        info[i].rob_entry.tick();
        info[i].calc_type.tick();
        info[i].busy.tick();
        info[i].ready.tick();
        info[i].Vj.tick();
        info[i].Vk.tick();
        info[i].A.tick();
        info[i].Qj.tick();
        info[i].Qk.tick();
        info[i].data_type.tick();
    }
}
void ReservationStation::set_flush() {
    need_flush = true;
}
void ReservationStation::flush() {

}
