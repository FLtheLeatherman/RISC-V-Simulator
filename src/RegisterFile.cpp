#include "RegisterFile.hpp"

void RegisterFile::init() {
    for (int i = 0; i < REG_NUM; ++i) {
        registers[i] = 0;
        busy[i] = false;
        tag[i] = -1;
    }
}
void RegisterFile::tick() {
    // if (need_flush) {
    //     flush();
    // }
    need_flush.tick();
    for (int i = 0; i < REG_NUM; ++i) {
        need_write[i].tick();
        need_clear[i].tick();
        if (need_write[i]) {
            busy[i] = true;
        } else if (need_clear[i]) {
            busy[i] = false;
            tag[i] = -1;
        }
        need_write[i] = false;
        need_clear[i] = false;
        busy[i].tick();
        tag[i].tick();
        registers[i].tick();
    }
}
void RegisterFile::write_tag(uint32_t id, int rob_entry) {
    if (id == 0) return;
    need_write[id] = true;
    tag[id] = rob_entry;
    busy[id] = true;
}
int RegisterFile::read_tag(uint32_t id) {
    return tag[id];
}
uint32_t RegisterFile::read_reg(uint32_t id) {
    return id == 0 ? 0 : registers[id];
}
void RegisterFile::write_reg(uint32_t reg, uint32_t val, int rob_entry) {
    if (reg == 0) return;
    registers[reg] = val;
    if (busy[reg] && tag[reg] == rob_entry) {
        need_clear[reg] = true;
    }
}
void RegisterFile::set_flush() {
    need_flush = true;
}
void RegisterFile::flush() {
    for (int i = 0; i < REG_NUM; ++i) {
        busy[i] = false;
        tag[i] = 0;
    }
    need_flush = false;
}
bool RegisterFile::readable(uint32_t id, int rob_entry) {
    return id == 0 || (!busy[id] || tag[id] == rob_entry);
}
void RegisterFile::run() {
    if (need_flush) {
        flush();
    }
}
void RegisterFile::print() {
    for (int i = 0; i < REG_NUM; ++i) {
        std::cout << i << ':' << registers[i] << ' ' << busy[i] << ' ' << tag[i] << std::endl;
    }
}