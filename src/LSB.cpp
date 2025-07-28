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
    need_flush.tick();
    head.tick(), tail.tick();
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        cir_que[i].ready.tick();
        cir_que[i].busy.tick();
    }
}
void LoadStoreBuffer::set_flush() {
    need_flush = true;
}
void LoadStoreBuffer::flush() {
    need_flush = false;
    head = 0;
    tail = 0;
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        cir_que[i].busy = false;
        cir_que[i].ready = false;
    }
}
void LoadStoreBuffer::run() {
    if (need_flush) {
        flush();
        return;
    }
    if (cir_que[head].busy && cir_que[head].ready) {
        cir_que[head].busy = false;
        cir_que[head].ready = false;
        head = (head + 1) % BUFFER_SIZE;
    }
}