/*
 * Copyright (C) 2019-2021 DBIS Group - TU Ilmenau, All Rights Reserved.
 *
 * This file is part of the Poseidon package.
 *
 * Poseidon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Poseidon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Poseidon. If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <functional>
#include <math.h>
#include "htable.hpp"
#ifdef USE_PFILE
#include "paged_string_pool.hpp"
#else
#include "string_pool.hpp"
#endif

inline uint16_t probe_distance(uint64_t val) { return (val & 0xFFFF000000000000) >> 48; }

inline dcode_t hash_value(uint64_t val) { return static_cast<dcode_t>(val & 0x00000000FFFFFFFF); }

inline uint64_t combine(dcode_t val, uint16_t dist) { return hash_value(val) | (static_cast<uint64_t>(dist) << 48); }

#if 0
/*compute the first prime number higher or equal to n*/
inline size_t nearest_prime(size_t n) {
    size_t prime = n;
    size_t sqrt_prime, i;
    while (true) {
        if (prime % 2 !=0) {
            sqrt_prime = (size_t)(sqrt(prime) + 1);
            for (i = 3; i < sqrt_prime; i += 2) {
                if (prime % i == 0)
                    break;
            }
            if (i >= sqrt_prime)
                return prime;
        }
        prime++;
    }
    return prime;
}

uint64_t step(const std::string& s, uint64_t prime) {
    uint64_t hash;
    const char *key = s.c_str();
    size_t len = s.length();
    size_t i;
    for (hash = (uint64_t)len, i = 0; i < len; i++)
        hash = (hash << 5) ^(hash >> 27) ^ key[i];
    return hash;
    // auto r = (hash % prime);
    // return r != 0 ? r : 1;
}
#endif

#ifdef USE_PFILE
htable::htable(p_ptr<paged_string_pool> pool, uint32_t nb) : pool_(pool), nbuckets_(nb), nelems_(0) {
#else
htable::htable(p_ptr<string_pool> pool, uint32_t nb) : pool_(pool), nbuckets_(nb), nelems_(0) {
#endif
    table_ = new uint64_t[nbuckets_];
    memset(table_, 0, nbuckets_ * sizeof(uint64_t));
}

htable::~htable() {
    delete [] table_;
}

void htable::rebuild() {
    pool_->scan([this](const char *s, dcode_t c) {
        insert(std::string(s), c);
    });
}

void htable::print() const {
    for (auto i = 0ul; i < nbuckets_; i++) {
        std::cout << "#" << i << " : " << hash_value(table_[i]) << " - dist=" << probe_distance(table_[i]) << std::endl;
    }
}

dcode_t htable::find(const std::string& s) {
    auto key = std::hash<std::string>{}(s);
    auto bucket_id = key % nbuckets_;
    auto start = bucket_id;
    auto v = 0ul;

    while(true) {
        v = table_[bucket_id];
        if (v == 0)
            break;
        else if (pool_->equal(hash_value(v), s))
            return bucket_id;
        // handle collision
        bucket_id += 1;
        if (bucket_id == start)
            break;
        bucket_id %= nbuckets_;
        // std::cout << "htable::find -> COLLISION - bucket_id = " << bucket_id << std::endl;
    };
    return UNKNOWN_CODE;
}

dcode_t htable::get(dcode_t id) {
    assert(id < nbuckets_);
    return hash_value(table_[id]);
}

dcode_t htable::insert(const std::string& s, dcode_t id) {
    auto fill = ((double) nelems_ / (double) nbuckets_);
    if (fill > 0.85)
        resize();
    auto key = std::hash<std::string>{}(s);
    nelems_++;
    return insert_into_table(table_, nbuckets_, key, id);
}

dcode_t htable::insert_into_table(uint64_t *tbl, uint32_t tsize, uint64_t hkey, dcode_t id) {
    auto bucket_id = hkey % tsize;
    auto start = bucket_id;
    bool overflow = false;
    uint16_t probe_dist = 0;

    while (true) {
        if (hash_value(tbl[bucket_id]) == 0)
            break;
        // handle collision
        bucket_id += 1;
        overflow = overflow ? overflow : bucket_id >= tsize;
        bucket_id %= tsize;
        if (overflow && bucket_id >= start) {
            std::cerr << "hash table overflow - aborting!" << std::endl;
            exit(-1);
        }
        auto other_dist = probe_distance(tbl[bucket_id]);
        if (other_dist < probe_dist) {
            auto rich_id = hash_value(tbl[bucket_id]);
            tbl[bucket_id] = combine(id, probe_dist);
            id = rich_id;
            probe_dist = other_dist;
        }
        else
            probe_dist += 1;
    }
    tbl[bucket_id] = combine(id, probe_dist);
    return bucket_id;
}

void htable::resize() {
    std::cout << "htable::resize..." << std::endl;
    auto nbuckets = nbuckets_ + nbuckets_/2;
    auto new_table_ = new uint64_t[nbuckets];
    memset(new_table_, 0, nbuckets * sizeof(uint64_t));
    for (auto i = 0ul; i < nbuckets_; i++) {
        if (table_[i] != 0) {
            // get the string
            auto s = pool_->extract(hash_value(table_[i]));
            // rehash
            auto key = std::hash<const char *>{}(s);
            // insert into new_table_
            insert_into_table(new_table_, nbuckets, key, table_[i]);
        }
    }
    delete [] table_;
    nbuckets_ = nbuckets;
    table_ = new_table_;
}
