#include "temporal_index.hpp"
#include "btree.hpp"
#include <type_traits>

// insert version to the temporal index
template <typename BTreePtr>
bool TemporalIndex<BTreePtr>::insert_version(uint32_t lid, uint32_t vt_start,
                                             TI_PtrType type, offset_t payload56, bool is_rel) {
    const uint64_t k = ti_encode_key(lid, vt_start, is_rel); // Pass is_rel here
    const uint64_t v = ti_encode_val(type, payload56); // Encode type and payload
    btree_->insert(k, v); // Insert into B-Tree
    return true; // Indicate success
}

// exact lookup in the temporal index
template <typename BTreePtr>
bool TemporalIndex<BTreePtr>::lookup_exact(uint32_t lid, uint32_t vt_start,
                                           TI_PtrType* out_type,
                                           offset_t* out_payload, bool is_rel) const {
    const uint64_t k = ti_encode_key(lid, vt_start, is_rel); // Pass is_rel here
    uint64_t v = 0; // Variable to hold retrieved value
    if (!btree_->lookup(k, &v)) return false; // Return false if not found
    if (out_type)    *out_type    = ti_decode_type(v); // Decode and set type
    if (out_payload) *out_payload = ti_decode_payload(v); // Decode and set payload
    return true;
}

// as-of lookup in the temporal index
template <typename BTreePtr>
bool TemporalIndex<BTreePtr>::lookup_as_of(uint32_t lid, uint32_t t,
                                           TI_PtrType* out_type,
                                           offset_t* out_payload,
                                           uint32_t start_vt_hint, bool is_rel) const {
    bool found = false; // Flag to indicate if a valid version is found
    uint64_t best_val = 0; // Variable to hold the best matching value

    // Encode boundaries using the partition bit
    const uint64_t min_k = ti_encode_key(lid, start_vt_hint, is_rel); // Start from hint
    const uint64_t max_k = ti_encode_key(lid, t, is_rel); // Up to valid time t

    // Scan only within the LID's timeline in the correct partition
    btree_->scan(min_k, max_k, [&](const uint64_t key, const uint64_t val) {
        // Double check LID and Partition bit to avoid boundary leaks
        if (ti_decode_lid(key) == lid) {
            found = true; // Mark that found at least one valid version
            best_val = val; // Update best value (latest version <= t)
        }
    });

    if (!found) return false; // Return false if no valid version found

    if (out_type)    *out_type    = ti_decode_type(best_val); // Decode and set type
    if (out_payload) *out_payload = ti_decode_payload(best_val); // Decode and set payload
    return true;
}
// template instantiation for supported B-Tree pointer types
template class TemporalIndex<im_btree_ptr>;
template class TemporalIndex<pf_btree_ptr>;
#ifdef USE_PMDK
template class TemporalIndex<nvm_btree_ptr>;
#endif