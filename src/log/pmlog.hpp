#ifndef pmlog_hpp_
#define pmlog_hpp_

#include "defs.hpp"
#include "transaction.hpp"


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

    pmlog();
    ~pmlog();
    
    id_t transaction_begin(xid_t txid);
    void transaction_end(id_t log_id);

    void append(id_t log_id, uint8_t *log_entry, int lsize);
 
private:
    p_ptr<log_chunk[]> ulog_; 
    p<std::size_t> nlogs_;
};

#endif