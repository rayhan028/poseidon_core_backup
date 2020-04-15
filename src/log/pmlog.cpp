#include <limits>
#include "pmlog.hpp"

pmlog::pmlog() {
    nlogs_ = 50;
    ulog_ = pmem::obj::make_persistent<log_chunk[]>(nlogs_);
}

pmlog::~pmlog() {
}
    
pmlog::id_t pmlog::transaction_begin(xid_t txid) {
    // find the first empty slot in ulog_ and return its index as log_id
    for (std::size_t i = 0; i < nlogs_; i++)
        if (ulog_[i].txid_ == 0) {
            // TODO: use mutex!!
            ulog_[i].txid_ = txid;
            pop.persist(ulog_[i].txid_);
            return i;
        }
    // TODO: handle the case of more than 50 active transactions
    return std::numeric_limits<std::size_t>::max();
}

void pmlog::transaction_end(pmlog::id_t log_id) {
    auto pop = pmem::obj::pool_by_vptr(this);
    // delete all additional log_chunks in ulog_[log_id]
    // TODO

    // finally, mark the slot as available
    // TODO: use mutex!!
    pop.memset_persist(&ulog_[log_id], 0, 4096);
}

void pmlog::append(id_t log_id, uint8_t *log_entry, int lsize) {
    auto& entry = ulog_[log_id];
    auto pop = pmem::obj::pool_by_vptr(this);
    if (4076 - entry.used > lsize) {
        entry.used -= lsize;
        pop.persist(entry.used);
        pop.memcpy_persist(&entry + used, log_entry, lsize);
    }
    else {
        // TODO
    }
}


