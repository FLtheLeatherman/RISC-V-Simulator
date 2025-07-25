#include "RegisterFile.hpp"

RegisterFile::RegisterFile() {
    for (int i = 0; i < REG_NUM; ++i) {
        registers[i].set_val(0);
        registers[i].set_nxt(0);
        tag[i] = -1;
    }
}
void RegisterFile::tick() {
    for (int i = 0; i < REG_NUM; ++i) {
        registers[i].tick();
    }
}
void RegisterFile::set_stat(uint32_t id, int robId) {
    tag[id] = robId;
}
void RegisterFile::set_val(uint32_t id, uint32_t val) {
    registers[id].set_nxt(val);
}
bool RegisterFile::is_busy(uint32_t id) {
    return (id != 0) && tag[id] >= 0;
}
int RegisterFile::get_tag(uint32_t id) {
    return tag[id];
}
uint32_t RegisterFile::get_val(uint32_t id) {
    return registers[id].get_val();
}