#include "ROB.hpp"

void ReorderBuffer::init(ReservationStation *rs_, Predictor *bp_, LoadStoreBuffer *lsb_, InstructionQueue *iq_, RegisterFile *rf_, Memory *mem_) {
    rs = rs_;
    bp = bp_;
    lsb = lsb_;
    iq = iq_;
    rf = rf_;
    mem = mem_;
    head = 0, tail = 0;
    halt = false;
    need_flush = false;
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        cir_que[i].busy = false;
        cir_que[i].ready = false;
        cir_que[i].type = RoBType::kErr;
        cir_que[i].val = 0;
        cir_que[i].dest = 0;
        cir_que[i].pc = 0;
        cir_que[i].lsb_entry = 0;
    }
    tick();
}
void ReorderBuffer::tick() {
    // if (need_flush) {
    //     flush();
    // } else {
    //     run();
    // }
    head.tick(), tail.tick();
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        // if (i == 4) {
        //     std::cout << '?' << cir_que[i].type.get_val() << ' ' << cir_que[i].type.get_nxt() << '\n';
        // }
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
    // cir_que[tail].type.set_nxt(type);
    // cir_que[tail].type.tick();
    // std::cout << "!!" << tail << std::endl;
    // std::cout << "!!" << type << ' ' << std::hex << pc << std::dec << std::endl;
    // std::cout << cir_que[tail].type.get_val() << ' ' << cir_que[tail].type.get_nxt() << std::endl;
    cir_que[tail].dest = dest;
    cir_que[tail].val = val;
    cir_que[tail].pc = pc;
    cir_que[tail].ready = false;
    cir_que[tail].busy = true;
    int res = tail;
    tail = (tail + 1) % BUFFER_SIZE;
    if (type == RoBType::kReg || type == RoBType::kJalr) {
        rf->write_tag(dest, res);
    }
    return res;
}
void ReorderBuffer::update(int rob_entry) {
    cir_que[rob_entry].ready = true;
    rs->update(rob_entry, cir_que[rob_entry].val);
}
void ReorderBuffer::update(int rob_entry, uint32_t val, uint32_t dest) {
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
void ReorderBuffer::update_without_branch(int rob_entry, uint32_t val) {
    cir_que[rob_entry].val = val;
    cir_que[rob_entry].ready = true;
}
void ReorderBuffer::commit() {
    if (!cir_que[head].busy || !cir_que[head].ready) return;
    bool flush_flag = false;
    // std::cout << "commit: " << head << '\n';
    // std::cout << cir_que[head].type << '\n';
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
            // std::cout << "Reg!" << '\n';
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
            // std::cout << head << ' ' << cir_que[head].dest << ' ' << cir_que[head].val << std::endl;
            mem->write_word(cir_que[head].dest, cir_que[head].val);
            // std::cout << "aa?\n";
            lsb->update(cir_que[head].lsb_entry);
            break;
        case RoBType::kJalr:
            rf->write_reg(cir_que[head].dest, cir_que[head].pc + 4, head);
            rs->update(head, cir_que[head].pc + 4);
            iq->update_pc(cir_que[head].val);
            flush_all();
            break;
        case RoBType::kHalt:
            std::cout << (rf->read_reg(10) & 0xff) << '\n'; 
            halt = true;
            break;
        default:
            exit(-1);
            break;
    }
    cir_que[head].busy = false;
    cir_que[head].ready = false;
    cir_que[head].type = RoBType::kErr;
    head = (head + 1) % BUFFER_SIZE;
    if (flush_flag) flush_all();
}
void ReorderBuffer::run() {
    if (need_flush) {
        flush();
        return;
    }
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
        cir_que[head].type = RoBType::kErr;
    }
    need_flush = false;
}
void ReorderBuffer::flush_all() {
    // std::cout << '!' << "flush" << '\n';
    rf->set_flush();
    iq->set_flush();
    rs->set_flush();
    lsb->set_flush();
    set_flush();
}
uint32_t ReorderBuffer::get_val(int rob_entry) {
    return cir_que[rob_entry].val;
}
bool ReorderBuffer::is_halt() {
    return halt;
}
void ReorderBuffer::print() {
    std::cout << "ROB:\n";
    // int i = head;
    // do {
    // for (int i = 0; i < BUFFER_SIZE; ++i) {
        // if (!cir_que[i].busy) return;
        int i = 4;
        std::cout << i << ": " << std::hex << cir_que[i].pc << std::dec << ' ' << cir_que[i].dest << ' ' << cir_que[i].ready << ' ' << cir_que[i].val << ' ' << cir_que[i].type.get_val() << ' ' << cir_que[i].type.get_nxt() << std::endl;
        // i = (i + 1) % BUFFER_SIZE;
    // }
    // } while(i != tail);
}