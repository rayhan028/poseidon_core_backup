#pragma once
#include <cstdint>
#include <memory>
#include "btree.hpp"
#include <type_traits>

// KEY ENCODING
static inline uint64_t ti_encode_key(uint32_t lid, uint32_t vt_start, bool is_rel = false) {
    uint64_t partition = is_rel ? 1ULL : 0ULL;
    return (partition << 63) | (static_cast<uint64_t>(lid & 0x7FFFFFFF) << 32) | static_cast<uint64_t>(vt_start);
}

// Decode lid from key
static inline uint32_t ti_decode_lid(uint64_t key) {
    return static_cast<uint32_t>((key >> 32) & 0x7FFFFFFF);
}

// Decode vt_start from key
static inline uint32_t ti_decode_vt_start(uint64_t key) {
    return static_cast<uint32_t>(key & 0xFFFFFFFFu);
}

// VALUE ENCODING
enum class TI_PtrType : uint8_t { HOT = 1, HIST = 2 }; // 8 bits for type

// Encode type and payload into value
static inline uint64_t ti_encode_val(TI_PtrType t, offset_t payload56) { // 56 bits for payload
    return (static_cast<uint64_t>(t) << 56) | (static_cast<uint64_t>(payload56) & 0x00FFFFFFFFFFFFFFULL); // Combine type and payload
}

// Decode type from value
static inline TI_PtrType ti_decode_type(uint64_t val) { // Extract top 8 bits
    return static_cast<TI_PtrType>((val >> 56) & 0xFF); // Extract type
}

// Decode payload from value
static inline offset_t ti_decode_payload(uint64_t val) { // Extract lower 56 bits
    return static_cast<offset_t>(val & 0x00FFFFFFFFFFFFFFULL); // Extract payload
}

template<typename BTreePtr>
class TemporalIndex {
public:
    explicit TemporalIndex(BTreePtr base) : btree_(std::move(base)) {} // Initialize with B-Tree instance

    // Insert a new version entry into the temporal index
    bool insert_version(uint32_t lid, uint32_t vt_start, TI_PtrType type, offset_t payload56, bool is_rel = false);

    // Remove a version entry from the temporal index
    void remove_version(uint32_t logical_id, uint32_t vt_start, bool is_rel = false) {
        uint64_t key = ti_encode_key(logical_id, vt_start, is_rel);
        this->btree_->erase(key);
    }

    // Exact lookup for a specific version
    bool lookup_exact(uint32_t lid, uint32_t vt_start, TI_PtrType* out_type, offset_t* out_payload, bool is_rel = false) const;

    // Lookup the latest version as of a given valid time
    bool lookup_as_of(uint32_t lid, uint32_t t, TI_PtrType* out_type, offset_t* out_payload,
                      uint32_t start_vt_hint = 0, bool is_rel = false) const;

    template <typename F>
    void scan_range(uint32_t lid, uint32_t a, uint32_t b, bool is_rel, F&& fn) const {
        const uint64_t start_k = ti_encode_key(lid, a, is_rel);
        const uint64_t end_k   = ti_encode_key(lid, b, is_rel);
        btree_->scan(start_k, end_k, [&](const uint64_t key, const uint64_t val) {
            fn(ti_decode_vt_start(key), ti_decode_type(val), ti_decode_payload(val));
        });
    }

    BTreePtr get_btree() const { return btree_; } // Access underlying B-Tree

private:
    BTreePtr btree_; // Underlying B-Tree instance
};