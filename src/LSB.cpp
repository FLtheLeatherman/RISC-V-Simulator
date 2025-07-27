#include "LSB.hpp"

LoadStoreBuffer::LoadStoreBuffer() {
    head = 0;
    tail = 0;
}
bool LoadStoreBuffer::available() {
    return !cir_que[tail].busy;
}
int LoadStoreBuffer::insert() {
    cir_que[tail].ready = false;
    cir_que[tail].busy = true;
    int res = tail;
    tail = (tail + 1) % BUFFER_SIZE;
    return res;
}
void LoadStoreBuffer::update(int id) {
    cir_que[id].ready = true;
}
bool LoadStoreBuffer::is_head(int id) {
    return id == head;
}
void LoadStoreBuffer::tick() {
    head.tick(), tail.tick();
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        cir_que[i].ready.tick();
        cir_que[i].busy.tick();
    }
}