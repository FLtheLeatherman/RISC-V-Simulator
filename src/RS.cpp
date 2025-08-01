#include "RS.hpp"

void ReservationStation::init(ALU *alu_, RegisterFile *rf_, ReorderBuffer *rob_, LoadStoreBuffer *lsb_, Memory *mem_) {
    alu = alu_;
    rf = rf_;
    rob = rob_;
    lsb = lsb_;
    mem = mem_;
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
    if (data_type == DataType::kTwoReg) {
        if (rf->readable(val1, rob_entry)) {
            info[pos].Vj = rf->read_reg(val1);
            info[pos].Qj = -1;
        } else {
            info[pos].Qj = rf->read_tag(val1);
        }
        if (rf->readable(val2, rob_entry)) {
            info[pos].Vk = rf->read_reg(val2);
            info[pos].Qk = -1;
        } else {
            info[pos].Qk = rf->read_tag(val2);
        }
    } else if (data_type == DataType::kRegImm) {
        info[pos].A = val2;
        if (rf->readable(val1, rob_entry)) {
            info[pos].Vj = rf->read_reg(val1);
            info[pos].Qj = -1;
        } else {
            info[pos].Qj = rf->read_tag(val1);
        }
        info[pos].Qk = -1;
    } else if (data_type == DataType::kTwoRegImm) {
        if (rf->readable(val1, rob_entry)) {
            info[pos].Vj = rf->read_reg(val1);
            info[pos].Qj = -1;
        } else {
            info[pos].Qj = rf->read_tag(val1);
        }
        if (rf->readable(val2, rob_entry)) {
            info[pos].Vk = rf->read_reg(val2);
            info[pos].Qk = -1;
        } else {
            info[pos].Qk = rf->read_tag(val2);
        }
        info[pos].A = val3;
    }
}
void ReservationStation::update(int rob_entry, uint32_t val) {
    need_update = true;
    update_entry = rob_entry, update_val = val;
    for (int i = 0; i < STATION_SIZE; ++i) {
        if (info[i].busy) {
            if (info[i].Qj == rob_entry) {
                info[i].Vj = val;
                info[i].Qj = -1;
            }
            if (info[i].Qk == rob_entry) {
                info[i].Vk = val;
                info[i].Qk = -1;
            }
        }
    }
}
void ReservationStation::update() {
    if (!need_update) return;
    for (int i = 0; i < STATION_SIZE; ++i) {
        if (info[i].busy) {
            if (info[i].Qj == update_entry) {
                info[i].Vj = update_val;
                info[i].Qj = -1;
            }
            if (info[i].Qk == update_entry) {
                info[i].Vk = update_val;
                info[i].Qk = -1;
            }
        }
    }
    need_update = false;
}
void ReservationStation::run() {
    if (need_flush) {
        flush();
        return;
    }
    for (int i = 0; i < STATION_SIZE; ++i) {
        if (info[i].busy && info[i].Qj == -1 && info[i].Qk == -1) {
            uint32_t res = 0;
            if ((int)info[i].calc_type < 14) {
                if (info[i].data_type == DataType::kTwoReg) {
                    res = alu->run(info[i].calc_type, info[i].Vj, info[i].Vk);
                } else if (info[i].data_type == DataType::kRegImm) {
                    res = alu->run(info[i].calc_type, info[i].Vj, info[i].A);
                }
                rob->update(info[i].rob_entry, res);
                info[i].busy = false;
            } else if ((int)info[i].calc_type < 19) {
                if (info[i].data_type == kRegImm) {
                    info[i].A = alu->run(CalcType::kAdd, info[i].Vj, info[i].A);
                    info[i].data_type = DataType::kImm;
                    info[i].Vj = rob->get_val(info[i].rob_entry);
                } else {
                    if (lsb->is_head(info[i].Vj)) {
                        if (info[i].calc_type == CalcType::kLb || info[i].calc_type == CalcType::kLbu) {
                            res = mem->read(info[i].A);
                            if (info[i].calc_type == CalcType::kLbu) {
                                res &= 0xFF;
                            }
                        } else if (info[i].calc_type == CalcType::kLh || info[i].calc_type == CalcType::kLhu) {
                            res = mem->read_half_word(info[i].A);
                            if (info[i].calc_type == CalcType::kLbu) {
                                res &= 0xFFFF;
                            }
                        } else if (info[i].calc_type == CalcType::kLw) {
                            res = mem->read_word(info[i].A);
                        }
                        lsb->update(info[i].Vj);
                        rob->update(info[i].rob_entry, res);
                        info[i].busy = false;
                    } else {
                        continue;
                    }
                }
            } else {
                // std::cout << "!" << "store" << '\n';
                res = alu->run(CalcType::kAdd, info[i].Vk, info[i].A);
                // std::cout << res << '\n';
                rob->update(info[i].rob_entry, info[i].Vj, res);
                info[i].busy = false;
            }
            break;
        }
    }
}
void ReservationStation::tick() {
    // if (need_flush) {
    //     flush();
    // } else {
    //     run();
    // }
    need_flush.tick();
    need_update.tick();
    update_entry.tick(), update_val.tick();
    for (int i = 0; i < STATION_SIZE; ++i) {
        info[i].rob_entry.tick();
        info[i].calc_type.tick();
        info[i].busy.tick();
        info[i].Vj.tick();
        info[i].Vk.tick();
        info[i].A.tick();
        info[i].Qj.tick();
        info[i].Qk.tick();
        info[i].data_type.tick();
    }
    update();
}
void ReservationStation::set_flush() {
    need_flush = true;
}
void ReservationStation::flush() {
    for (int i = 0; i < STATION_SIZE; ++i) {
        info[i].busy = false;
        info[i].Vj = 0, info[i].Vk = 0;
        info[i].Qj = -1, info[i].Qk = -1;
        info[i].A = 0;
        info[i].rob_entry = -1;
    }
    need_flush = false;
}
void ReservationStation::print() {
    std::cout << "RS:\n";
    std::cout << "id\t\trob_entry\t\tVj\t\tVk\t\tQj\t\tQk\t\tA\n";
    for (int i = 0; i < STATION_SIZE; ++i) {
        if (!info[i].busy) continue;
        std::cout << i << "\t\t" << info[i].rob_entry << "\t\t" << info[i].Vj << "\t\t" << info[i].Vk << "\t\t" << info[i].Qj << "\t\t" << info[i].Qk << "\t\t" << info[i].A << '\n';
    }
}