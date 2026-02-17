#ifndef HISTORY_TYPES_HPP
#define HISTORY_TYPES_HPP

#include "defs.hpp"
#include "properties.hpp"
#include <vector>
#include <unordered_map>
#include <cstdint>

struct p_item;
using offset_t = uint64_t;

/**
 * node_history_delta
 */
struct node_history_delta {
    uint64_t   lid;
    dcode_t    label;
    uint64_t   vt_start;
    uint64_t   vt_end;
    offset_t   base_property_list;
    property_set::id_t delta_pid;
    offset_t   prev_version;
    uint64_t   tt_start;
    uint64_t   tt_end;
    offset_t from_rship_list;
    offset_t to_rship_list;
};

/**
 * rship_history_delta
 */
struct rship_history_delta {
    uint64_t   lid;
    dcode_t    label;
    node::logical_id_t src_lid;
    node::logical_id_t dest_lid;
    uint64_t   vt_start;
    uint64_t   vt_end;
    offset_t   base_property_list;
    property_set::id_t delta_pid;
    offset_t   prev_version;
    uint64_t   tt_start;
    uint64_t   tt_end;
    offset_t next_src;
    offset_t next_dest;
};

// STORAGE STRUCTS
struct node_history_record {
    uint64_t   lid;
    dcode_t    label;
    uint64_t   vt_start;
    uint64_t   vt_end;
    offset_t   base_property_list;
    property_set::id_t delta_pid;
    offset_t   prev_version;
    uint64_t   tt_start;
    uint64_t   tt_end;
    offset_t   from_rship_list;
    offset_t   to_rship_list;
};

struct rship_history_record {
    uint64_t   lid;
    dcode_t    label;
    node::logical_id_t src_lid;
    node::logical_id_t dest_lid;
    uint64_t   vt_start;
    uint64_t   vt_end;
    offset_t   base_property_list;
    property_set::id_t delta_pid;
    offset_t   prev_version;
    uint64_t   tt_start;
    uint64_t   tt_end;
    offset_t   next_src;
    offset_t   next_dest;
};

// CHECKPOINTS STRUCTS for node records
struct node_checkpoint {
    uint64_t   lid;
    uint64_t   vt;
    offset_t   base_property_list;
    property_set::id_t overlay_pid;

    offset_t  from_rship_list{static_cast<offset_t>(-1)}; 
    offset_t  to_rship_list{static_cast<offset_t>(-1)};
};

// CHECKPOINTS STRUCTS for relationship records
struct rship_checkpoint {
    uint64_t   lid;
    uint64_t   vt;
    offset_t   base_property_list;
    property_set::id_t overlay_pid;
};

#endif