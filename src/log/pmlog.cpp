#include <limits>
#include "pmlog.hpp"

pmlog::pmlog() {
    nlogs_ = 50;
#ifdef USE_PMDK
    ulog_ = pmem::obj::make_persistent<log_chunk[]>(nlogs_);
#else
    ulog_ = new log_chunk[nlogs_];
#endif
}

pmlog::~pmlog() {
#ifndef USE_PMDK
    delete [] ulog_;
#endif
}
    
pmlog::id_t pmlog::transaction_begin(xid_t txid) {
#ifdef USE_PMDK
    auto pop = pmem::obj::pool_by_vptr(this);
    // find the first empty slot in ulog_ and return its index as log_id
    for (std::size_t i = 0; i < nlogs_; i++)
        if (ulog_[i].txid_ == 0) {
            // TODO: use mutex!!
            ulog_[i].txid_ = txid;
            pop.persist(&(ulog_[i].txid_), sizeof(xid_t));
            return i;
        }
    // TODO: handle the case of more than 50 active transactions
#else
    for (std::size_t i = 0; i < nlogs_; i++)
        if (ulog_[i].txid_ == 0) {
            // TODO: use mutex!!
            ulog_[i].txid_ = txid;
            return i;
        }
    // TODO: handle the case of more than 50 active transactions
#endif
    return std::numeric_limits<std::size_t>::max();
}

void pmlog::transaction_end(pmlog::id_t log_id) {
#ifdef USE_PMDK
    auto pop = pmem::obj::pool_by_vptr(this);
#endif
    // delete all additional log_chunks in ulog_[log_id]
    // TODO

    // finally, mark the slot as available
    // TODO: use mutex!!
#ifdef USE_PMDK
    pop.memset_persist(&ulog_[log_id], 0, 4096);
#else
    memset(&(ulog_[log_id]), 0, 4096);
#endif
}

void pmlog::append(id_t log_id, void *log_entry, uint32_t lsize) {
    auto entry = &(ulog_[log_id]);
#ifdef USE_PMDK
    auto pop = pmem::obj::pool_by_vptr(this);
#endif
    if (4076 - entry->used_ > lsize) {
#ifdef USE_PMDK
        pop.memcpy_persist(entry + entry->used_, log_entry, lsize);
#else
        memcpy(entry + entry->used_, log_entry, lsize);
#endif
        entry->used_ += lsize;
#ifdef USE_PMDK
        pop.persist(&(entry->used_), sizeof(int));
#endif
    }
    else {
        // TODO
    }
}

void pmlog::dump_chunk(log_chunk& log) {
    std::cout << "log for tx #" << log.txid_ << ", " << log.used_ << " bytes used." << std::endl;
    uint32_t pos = 0;
    while (pos < log.used_) {
        auto rec_ptr = (log_dummy *)(&(log.data_[pos]));
        if (rec_ptr->log_type == pmlog::log_insert) {
            auto ins_rec_ptr = (log_ins_record *)(&(log.data_[pos]));
            std::cout << "INSERT #" << ins_rec_ptr->oid << std::endl;
            pos += sizeof(log_ins_record);
        }
        else if (rec_ptr->log_type == pmlog::log_update) {
            if (rec_ptr->obj_type == pmlog::log_node) {
                auto upd_rec_ptr = (log_upd_node_record *)(&(log.data_[pos]));
                std::cout << "UPDATE #" << upd_rec_ptr->oid 
                            << ", UNDO={" << upd_rec_ptr->label  
                            << "}" << std::endl;
                pos += sizeof(log_upd_node_record);
            }
        }
	else 
		return;
    }
}

void pmlog::dump() {
    for (std::size_t i = 0; i < nlogs_; i++)
        if (ulog_[i].txid_ != 0) {
            auto& log = ulog_[i];
            dump_chunk(log);
        }
}
