#include "RegisterFile.hpp"

RegisterFile::RegisterFile() {
    for (int i = 0; i < REG_NUM; ++i) {
        registers[i] = 0;
        busy[i] = false;
        tag[i] = -1;
    }
}
void RegisterFile::tick() {
    if (need_flush) {
        flush();
    }
    need_flush.tick();
    for (int i = 0; i < REG_NUM; ++i) {
        write_tag[i].tick();
        clear_tag[i].tick();
        if (write_tag[i]) {
            busy[i] = true;
        } else if (clear_tag[i]) {
            busy[u] = false;
            tag[i] = -1;
        }
        write_tag[i] = false;
        clear_tag[i] = false;
        busy[i].tick();
        tag[i].tick();
        registers[i].tick();
    }
}
void RegisterFile::write_tag(uint32_t id, int rob_entry) {
    if (reg == 0) return;
    write_tag[id] = true;
    tag[id] = rob_entry;
    busy[reg] = true;
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
        clear_tag[reg] = true;
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