/*
 * Copyright (C) 2019-2022 DBIS Group - TU Ilmenau, All Rights Reserved.
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
#include <cstdlib>
#include "paged_string_pool.hpp"

paged_string_pool::paged_string_pool(bufferpool& bp, uint64_t fid) : 
    bpool_(bp), file_id_(fid), file_mask_(fid << 60) {
    npages_ = bpool_.get_file(file_id_)->num_pages();
    if (npages_ == 0) {
        // we have a new file
        bpool_.allocate_page(file_id_);
        npages_ = 1;
    }
}

void paged_string_pool::scan(std::function<void(const char *s, dcode_t c)> cb) {
    auto npage = 0u;
    uint32_t last = 0;

    bpool_.scan_file(file_id_, [&](auto pg) {
        auto data = &(pg->payload[0]);
        auto ppos = sizeof(uint32_t);
        memcpy(&last, data, sizeof(uint32_t));
        auto pos = npage * PAGE_SIZE + sizeof(uint32_t);

        for (auto p = sizeof(uint32_t); p < last; p++) {
            if (data[p] == '\0') {
                cb((const char *)&data[ppos], pos);
                pos = p + 1;
                ppos = p + 1;
            }
        }
        npage++;
    });  
    assert(npage == npages_);
}

const char *paged_string_pool::extract(dcode_t pos) const {
    paged_file::page_id pid = pos / PAGE_SIZE + 1;
    auto pg = bpool_.fetch_page(pid | file_mask_);
    auto page_offset = pos % PAGE_SIZE;
    return (const char *) &(pg->payload[page_offset]);
}

bool paged_string_pool::equal(dcode_t pos, const std::string& s) const {
    paged_file::page_id pid = pos / PAGE_SIZE + 1;
    auto pg = bpool_.fetch_page(pid | file_mask_);
    auto page_offset = pos % PAGE_SIZE;
    auto i = 0u;
    for (; i < s.length() && i + page_offset < PAGE_SIZE; i++)
        if (pg->payload[page_offset + i] != s.at(i))
            return false;
    return pg->payload[page_offset + i] == '\0';
}

dcode_t paged_string_pool::add(const std::string& str) {
    auto pg = bpool_.last_valid_page(file_id_);
    uint32_t last_pos = 0;
    
    memcpy(&last_pos, &(pg.first->payload[0]), sizeof(uint32_t));
    if (last_pos == 0) last_pos += sizeof(uint32_t);

    auto page_pos = last_pos - (npages_ - 1) * PAGE_SIZE;
    if (page_pos + str.length() + 1 >= PAGE_SIZE) {
        // we need a new page
        pg = bpool_.allocate_page(file_id_);
        npages_++;
        last_pos = (npages_ - 1) * PAGE_SIZE + sizeof(uint32_t);
        page_pos = sizeof(uint32_t);
    }
    dcode_t pos = last_pos/* + (npages_ - 1) * PAGE_SIZE*/;
    memcpy(&(pg.first->payload[page_pos]), str.c_str(), str.length());
    last_pos += str.length() + 1;
    page_pos += str.length() + 1; 
    pg.first->payload[page_pos - 1] = '\0';
    memcpy(&(pg.first->payload[0]), &last_pos, sizeof(uint32_t));
    // mark dirty
    bpool_.mark_dirty(pg.second | file_mask_);
    return pos;
}

void paged_string_pool::print() const {
    // std::cout << std::string(pool_, last_) << std::endl;
}
