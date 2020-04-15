#ifndef pmlog_hpp_
#define pmlog_hpp_

#include "defs.hpp"
#include "transaction.hpp"


struct log_dummy {
    uint8_t log_type : 3; // log_entry_type
    uint8_t obj_type : 2; // log_object_type
};

struct log_ins_record {
    uint8_t log_type : 3; // log_entry_type
    uint8_t obj_type : 2; // log_object_type
    offset_t oid;
};

struct log_upd_node_record {
    uint8_t log_type : 3; // log_entry_type
    uint8_t obj_type : 2; // log_object_type
    offset_t oid;
    dcode_t label;
    offset_t from_rship_list, to_rship_list, property_list;
};

class pmlog {
    using id_t = std::size_t;

    struct log_chunk {
        uint8_t data_[4076];
        xid_t txid_;
        uint32_t used_;
        p_ptr<log_chunk> next_;
    };

    using chunk_ptr = p_ptr<log_chunk>;

public:
enum log_entry_type {
    log_insert = 1,
    log_update = 2,
    log_delete = 3  
};

enum log_object_type {
    log_node = 1,
    log_rship = 2,
    log_property = 3
};


struct log_rec_iter {
#ifdef USE_PMDK
    log_rec_iter(p_ptr<log_chunk> ptr = nullptr) : chunk_(ptr), pos_(0) {}
#else
    log_rec_iter(log_chunk* ptr = nullptr) : chunk_(ptr), pos_(0) {}
#endif
    bool operator!= (const log_rec_iter& other) const { return chunk_ != other.chunk_ || pos_ != other.pos_; }
    log_rec_iter& operator++() { /* TODO */ return *this; }

    log_entry_type log_type() const;
    log_object_type obj_type() const;

    template <typename T> T& get();

#ifdef USE_PMDK
    p_ptr<log_chunk> chunk_;
#else
    log_chunk* chunk_;
#endif
    std::size_t pos_;
};

class log_iter {
public:
#ifdef USE_PMDK
    log_iter(p_ptr<log_chunk[]> l, std::size_t m, std::size_t p = 0) : log_(l), cpos_(p), maxpos_(m) {}
#else
    log_iter(log_chunk* l, std::size_t m, std::size_t p = 0) : log_(l), cpos_(p), maxpos_(m) {}
#endif
    bool operator!= (const log_iter& other) const { return log_ != other.log_ || cpos_ != other.cpos_; }
    log_iter& operator++() { cpos_++; return *this; }
    bool valid() const { return cpos_ < maxpos_ && log_[cpos_].txid_ != 0; }

    xid_t txid() const { return log_[cpos_].txid_; }

    log_rec_iter begin() { return log_rec_iter(&(log_[cpos_])); }
    log_rec_iter end() { return log_rec_iter(); }

private:
#ifdef USE_PMDK
    p_ptr<log_chunk[]> log_;
#else
    log_chunk *log_;
#endif
    std::size_t cpos_, maxpos_;
};

    pmlog();
    ~pmlog();
    
    id_t transaction_begin(xid_t txid);
    void transaction_end(id_t log_id);

    void append(id_t log_id, void *log_entry, uint32_t lsize);
 
    log_iter log_begin() { return log_iter(ulog_, nlogs_); }
    log_iter log_end() { return log_iter(ulog_, nlogs_, nlogs_); }

    void dump();

private:
    void dump_chunk(log_chunk& log);

#ifdef USE_PMDK
    p_ptr<log_chunk[]> ulog_; 
#else   
    log_chunk *ulog_;
#endif
    p<std::size_t> nlogs_;
};

#endif
