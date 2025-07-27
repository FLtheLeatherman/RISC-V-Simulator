#include "ROB.hpp"

ReorderBuffer::ReorderBuffer(ReservationStation *rs_, Predictor *bp_, LoadStoreBuffer *lsb_, InstructionQueue *iq_, RegisterFile *rf_) {
    rs = rs_;
    bp = bp_;
    lsb = lsb_;
    iq = iq_;
    rf = rf_;
    head = 0, tail = 0;
    halt = false;
    need_flush = false;
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        cir_que[i].busy = false;
        cir_que[i].ready = false;
    }
}
void ReorderBuffer::tick() {
    if (need_flush) {
        flush();
    } else {
        run();
    }
    head.tick(), tail.tick();
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        cir_que[i].ready.tick();
        cir_que[i].busy.tick();
        cir_que[i].type.tick();
        cir_que[i].val.tick();
        cir_que[i].dest.tick();
        cir_que[i].pc.tick();
        cir_que[i].lsb_entry.tick();
    }
    need_flush.tick();
    halt.tick();
}
bool ReorderBuffer::available() {
    return !cir_que[tail].busy;
}
int ReorderBuffer::insert(RoBType type, uint32_t val, uint32_t dest, uint32_t pc) {
    cir_que[tail].type = type;
    cir_que[tail].dest = dest;
    cir_que[tail].val = val;
    cir_que[tail].pc = pc;
    cir_que[tail].ready = false;
    cir_que[tail].busy = true;
    int res = tail;
    tail = (tail + 1) % BUFFER_SIZE;
    if (type == RoBType::kReg || type == RoBType::kJalr) {
        rf->set_stat(dest, res);
    }
    return res;
}
void ReorderBuffer::update(int rob_entry) {
    cir_que[rob_entry].ready = true;
    rs->update(rob_entry, cir_que[rob_entry].val);
}
void ReorderBuffer::update_store(int rob_entry, uint32_t val, uint32_t dest) {
    cir_que[rob_entry].ready = true;
    cir_que[rob_entry].lsb_entry = cir_que[rob_entry].val;
    cir_que[rob_entry].val = val;
    cir_que[rob_entry].dest = dest;
}
void ReorderBuffer::update(int rob_entry, uint32_t val) {
    if (cir_que[rob_entry].type == RoBType::kBranch) {
        if (val == cir_que[rob_entry].val) {
            cir_que[rob_entry].type = RoBType::kBranchSuccess;
        } else {
            cir_que[rob_entry].type = RoBType::kBranchFail;
        }
    }
    cir_que[rob_entry].val = val;
    cir_que[rob_entry].ready = true;
}
void ReorderBuffer::commit() {
    if (!cir_que[head].busy || !cir_que[head].ready) return;
    bool flush_flag = false;
    switch (cir_que[head].type) {
        case RoBType::kBranchSuccess:
            break;
        case RoBType::kBranchFail:
            if (cir_que[head].val == 0) {
                iq->update_pc(cir_que[head].pc + 4);
            } else {
                iq->update_pc(cir_que[head].dest);
            }
            flush_flag = true;
            break;
        case RoBType::kReg:
            rf->write_reg(cir_que[head].dest, cir_que[head].val, head);
            rs->update(head, cir_que[head].val);
            break;
        case RoBType::kStoreByte:
            if (!lsb->is_head(cir_que[head].lsb_entry)) return;
            mem->write(cir_que[head].dest, cir_que[head].val);
            lsb->update(cir_que[head].lsb_entry);
            break;
        case RoBType::kStoreHalf:
            if (!lsb->is_head(cir_que[head].lsb_entry)) return;
            mem->write_half_word(cir_que[head].dest, cir_que[head].val);
            lsb->update(cir_que[head].lsb_entry);
            break;
        case RoBType::kStoreWord:
            if (!lsb->is_head(cir_que[head].lsb_entry)) return;
            mem->write_word(cir_que[head].dest, cir_que[head].val);
            lsb->update(cir_que[head].lsb_entry);
            break;
        case RoBType::kJalr:
            rf->write_reg(cir_que[head].dest, cir_que[head].pc + 4, head);
            rs->update(head, cir_que[head].pc + 4);
            iq->update_pc(cir_que[head].val);
            flush_all();
            break;
        case RoBType::kHalt:
            std::cout << (rf->get_val(10) & 0xff) << '\n'; 
            halt = true;
            break;
        default:
            exit(-1);
            break;
    }
    cir_que[head].busy = false;
    cir_que[head].ready = false;
    head = (head + 1) % BUFFER_SIZE;
    if (flush_flag) flush_all();
}
void ReorderBuffer::run() {
    if (head == tail) return;
    if (cir_que[head].ready) {
        commit();
    }
}
void ReorderBuffer::set_flush() {
    need_flush = true;
}
void ReorderBuffer::flush() {
    head = 0, tail = 0;
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        cir_que[i].busy = false;
        cir_que[i].ready = false;
    }
    need_flush = false;
}
void ReorderBuffer::flush_all() {
    rf->set_flush();
    iq->set_flush();
    rs->set_flush();
    lsb->set_flush();
    set_flush();
}