/*
 * Copyright (C) 2019-2023 DBIS Group - TU Ilmenau, All Rights Reserved.
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

#include <boost/filesystem.hpp>
#include "walog.hpp"

wa_log::wa_log(const std::string& fname)  {
   boost::filesystem::path path_obj(fname);
    // check if path exists and is of a regular file
    if (! boost::filesystem::exists(path_obj)) {
        // create a new file
        log_fp_ = std::fopen(fname.c_str(), "w+b");
        header_.last_lsn_ = 0;
        auto res = std::fwrite((void *)&header_, 1, sizeof(header_), log_fp_);
        assert(res == sizeof(header_));
        ::fsync(log_fp_->_file);
    }
    else {
        // open an existing file
        log_fp_ = std::fopen(fname.c_str(), "r+b");     
        std::fseek(log_fp_, 0, SEEK_SET);
        auto res = std::fread((void *)&header_, 1, sizeof(header_), log_fp_);
        if (memcmp(header_.fid_, "PSLG", 4) || res != sizeof(header_)) {
            std::fclose(log_fp_);
            log_fp_ = nullptr;
            return;
        }
 
        std::fseek(log_fp_, 0, SEEK_END);
    }
    std::setbuf(log_fp_, nullptr);
}


wa_log::~wa_log() {
    close();
}

void wa_log::rewind() {
    std::fseek(log_fp_, sizeof(header_), SEEK_SET);
}

void wa_log::close() {
    if (log_fp_ != nullptr) {
        std::fseek(log_fp_, 0, SEEK_SET);
        std::fwrite((void *)&header_, 1, sizeof(header_), log_fp_);
        ::fsync(log_fp_->_file);
        std::fclose(log_fp_);
    }
    log_fp_ = nullptr;
}


void wa_log::transaction_begin(xid_t txid) {
    wal::log_tx_record rec(next_lsn(), txid, log_bot);
    last_offsets_.emplace(txid, std::ftell(log_fp_));
    append(static_cast<void *>(&rec), sizeof(rec)); 
    // spdlog::info("last_offset={}", std::ftell(log_fp_));
}


void wa_log::transaction_commit(xid_t txid) {
    wal::log_tx_record rec(next_lsn(), txid, log_commit);
    auto it = last_offsets_.find(txid);
    if (it != last_offsets_.end())
        rec.prev_offset = it->second;
    append(static_cast<void *>(&rec), sizeof(rec)); 
    last_offsets_.erase(txid);
}


void wa_log::transaction_abort(xid_t txid) {
    wal::log_tx_record rec(next_lsn(), txid, log_abort);
    auto it = last_offsets_.find(txid);
    if (it != last_offsets_.end())
        rec.prev_offset = it->second;
    append(static_cast<void *>(&rec), sizeof(rec)); 
    last_offsets_.erase(txid);
}

void wa_log::append(xid_t tx_id, wal::log_node_record &log_entry) { 
    log_entry.lsn = next_lsn(); 
    log_entry.tx_id = tx_id; 
    auto it = last_offsets_.find(tx_id);
    if (it != last_offsets_.end())
        log_entry.prev_offset = it->second;
    last_offsets_[tx_id] = std::ftell(log_fp_);
    append(static_cast<void *>(&log_entry), sizeof(log_entry)); 
    // spdlog::info("last_offset={}",last_offsets_[tx_id]);
}
    
void wa_log::append(xid_t tx_id, wal::log_rship_record &log_entry)  { 
    log_entry.lsn = next_lsn(); 
    log_entry.tx_id = tx_id; 
    auto it = last_offsets_.find(tx_id);
    if (it != last_offsets_.end())
        log_entry.prev_offset = it->second;
    last_offsets_[tx_id] = std::ftell(log_fp_);
    append(static_cast<void *>(&log_entry), sizeof(log_entry)); 
    // spdlog::info("last_offset={}", last_offsets_[tx_id]);
}  

void wa_log::append(void *log_entry, uint32_t lsize) {
    // spdlog::info("write record at {}", std::ftell(log_fp_));
    auto res = std::fwrite(log_entry, 1, lsize, log_fp_);
    // spdlog::info("\twrote {} bytes | {}", res, std::ftell(log_fp_));
    assert(res == lsize);
}

void wa_log::dump() {

}

uint64_t wa_log::next_lsn() {
    return ++(header_.last_lsn_);
}

void wa_log::fetch_record(offset_t pos) {
    wal::log_dummy rec;
    std::fseek(log_fp_, pos, SEEK_SET);
    auto res = std::fread((void *)&rec, 1, sizeof(rec), log_fp_);
    if (res == 0 && std::feof(log_fp_)) 
        return;
    std::size_t nbytes = 0;
    switch(rec.obj_type) {
    case log_none:
        // TX entry
        nbytes = sizeof(wal::log_tx_record);
        break;
    case log_node:
        nbytes = sizeof(wal::log_node_record);
        break;
    case log_rship:
        nbytes = sizeof(wal::log_rship_record);
        break;
    case log_property:
        break;
    }
    memcpy(buf_, &rec, sizeof(rec));
    std::fread((void *)&buf_[sizeof(rec)], 1, nbytes - sizeof(rec), log_fp_);
}

//----------------------------------------------------------------------

bool wa_log::log_iter::read_log_entry() {
    wal::log_dummy rec;
    current_pos_ = std::ftell(fp_);
    auto res = std::fread((void *)&rec, 1, sizeof(rec), fp_);
    if (res == 0 && std::feof(fp_)) {
        fp_ = nullptr;
        return false;
    }
    std::size_t nbytes = 0;
    switch(rec.obj_type) {
    case log_none:
        // TX entry
        nbytes = sizeof(wal::log_tx_record);
        break;
    case log_node:
        nbytes = sizeof(wal::log_node_record);
        break;
    case log_rship:
        nbytes = sizeof(wal::log_rship_record);
        break;
    case log_property:
        break;
    }
    memcpy(entry_, &rec, sizeof(rec));
    std::fread((void *)&entry_[sizeof(rec)], 1, nbytes - sizeof(rec), fp_);
    if (std::feof(fp_)) {
        fp_ = nullptr;
        return false;
    }
    return true;
}

wa_log::log_iter& wa_log::log_iter::operator++() {
    read_log_entry();
    return *this;
}

xid_t wa_log::log_iter::transaction_id() const {
    auto rec_ptr = get<wal::log_dummy>();
    return rec_ptr->tx_id;
}


log_entry_type wa_log::log_iter::log_type() const {
    auto rec_ptr = get<wal::log_dummy>();
    return (log_entry_type)rec_ptr->log_type;
}

log_object_type wa_log::log_iter::obj_type() const {
    auto rec_ptr = get<wal::log_dummy>();
    return (log_object_type)rec_ptr->obj_type;
}

uint64_t wa_log::log_iter::lsn() const {
    auto rec_ptr = get<wal::log_dummy>();
    return rec_ptr->lsn;
}