/*
 * Copyright (C) 2019-2020 DBIS Group - TU Ilmenau, All Rights Reserved.
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

#include "pm_ulog.hpp"
#include <limits>

pm_ulog::log_rec_iter &pm_ulog::log_rec_iter::operator++() {
  auto old = pos_;
  auto rec_ptr = (pmlog::log_dummy *)(&(chunk_->data_[pos_]));
  switch (rec_ptr->log_type) {
  case log_insert:
    pos_ += sizeof(pmlog::log_ins_record);
    break;
  case log_update:
    switch (rec_ptr->obj_type) {
    case log_node:
      pos_ += sizeof(pmlog::log_node_record);
      break;
    case log_rship:
      pos_ += sizeof(pmlog::log_rship_record);
      break;
    case log_property:
      pos_ += sizeof(pmlog::log_property_record);
      break;
    }
    break;
  case log_delete:
    // TODO
    break;
  }
  // if we cannot advance we arrived on the end
  if (old == pos_)
    pos_ = std::numeric_limits<std::size_t>::max();
  return *this;
}

log_entry_type pm_ulog::log_rec_iter::log_type() const {
  auto rec_ptr = (pmlog::log_dummy *)(&(chunk_->data_[pos_]));
  return (log_entry_type)rec_ptr->log_type;
}

log_object_type pm_ulog::log_rec_iter::obj_type() const {
  auto rec_ptr = (pmlog::log_dummy *)(&(chunk_->data_[pos_]));
  return (log_object_type)rec_ptr->obj_type;
}

void pm_ulog::log_rec_iter::set_invalid() {
  auto rec_ptr = (pmlog::log_dummy *)(&(chunk_->data_[pos_]));
  rec_ptr->valid_flag = false;
}

bool pm_ulog::log_rec_iter::valid() const { 
  auto rec_ptr = (pmlog::log_dummy *)(&(chunk_->data_[pos_]));
  return rec_ptr->valid_flag;
}

/* -------------------------------------------------------------------------- */

pm_ulog::log_chunk::log_chunk() : txid_(0), used_(0) {
#ifdef USE_PMDK
  auto pop = pmem::obj::pool_by_vptr(this);
  pop.memset_persist(data_, 0, 4076);
#else
  memset(data_, 0, 4076);
#endif
}

/* -------------------------------------------------------------------------- */

pm_ulog::pm_ulog() {
  nlogs_ = 50;
#ifdef USE_PMDK
  ulog_ = pmem::obj::make_persistent<log_chunk[]>(nlogs_);
#else
  ulog_ = new log_chunk[nlogs_];
#endif
}

pm_ulog::~pm_ulog() {
#ifndef USE_PMDK
  delete[] ulog_;
#endif
}


pm_ulog::id_t pm_ulog::transaction_begin(xid_t txid) {
#ifdef USE_PMDK
  auto pop = pmem::obj::pool_by_vptr(this);
  // find the first empty slot in ulog_ and return its index as log_id
  for (std::size_t i = 0; i < nlogs_; i++) {
    std::lock_guard<std::mutex> guard(lmtx_);
    if (ulog_[i].txid_ == 0) {
      ulog_[i].txid_ = txid;
      pop.persist(&(ulog_[i].txid_), sizeof(xid_t));
      return i;
    }
  }
    // TODO: handle the case of more than 50 active transactions
#else
  for (std::size_t i = 0; i < nlogs_; i++) {
    std::lock_guard<std::mutex> guard(lmtx_);
    if (ulog_[i].txid_ == 0) {
      ulog_[i].txid_ = txid;
      return i;
    }
  }
    // TODO: handle the case of more than 50 active transactions
#endif
  return std::numeric_limits<std::size_t>::max();
}

void pm_ulog::transaction_end(pm_ulog::id_t log_id) {
  // finally, mark the slot as available
  std::lock_guard<std::mutex> guard(lmtx_);

#ifdef USE_PMDK
  auto pop = pmem::obj::pool_by_vptr(this);
  pop.memset_persist(&ulog_[log_id], 0, sizeof(pm_ulog::log_chunk));
#else
  memset(&(ulog_[log_id].data_), 0, 4076);
  ulog_[log_id].next_ = nullptr;
  ulog_[log_id].used_ = 0;
  ulog_[log_id].txid_ = 0;
#endif
}

void pm_ulog::append(id_t log_id, void *log_entry, uint32_t lsize) {
  std::lock_guard<std::mutex> guard(lmtx_);
  assert(log_id < nlogs_);
  auto entry = &(ulog_[log_id]);
#ifdef USE_PMDK
  auto pop = pmem::obj::pool_by_vptr(this);
#endif
  if (4076 - entry->used_ > lsize) {
    auto addr = (void *)((uint8_t *)entry + entry->used_);
#ifdef USE_PMDK
    pop.memcpy_persist(addr, log_entry, lsize);
#else
    memcpy(addr, log_entry, lsize);
#endif
    entry->used_ += lsize;
#ifdef USE_PMDK
    pop.persist(&(entry->used_), sizeof(int));
#endif
  } else {
    // TODO
  }
}

void pm_ulog::dump_chunk(log_chunk &log) {
  std::cout << "log for tx #" << log.txid_ << ", " << log.used_
            << " bytes used." << std::endl;
  uint32_t pos = 0;
  while (pos < log.used_) {
    auto rec_ptr = (pmlog::log_dummy *)(&(log.data_[pos]));
    if (rec_ptr->log_type == log_insert) {
      auto ins_rec_ptr = (pmlog::log_ins_record *)(&(log.data_[pos]));
      std::cout << "INSERT #" << ins_rec_ptr->oid << std::endl;
      pos += sizeof(pmlog::log_ins_record);
    } else if (rec_ptr->log_type == log_update) {
      if (rec_ptr->obj_type == log_node) {
        auto upd_rec_ptr = (pmlog::log_node_record *)(&(log.data_[pos]));
        std::cout << "UPDATE #" << upd_rec_ptr->oid << ", UNDO={"
                  << upd_rec_ptr->label << "}" << std::endl;
        pos += sizeof(pmlog::log_node_record);
      }
    } else
      return;
  }
}

void pm_ulog::dump() {
  for (std::size_t i = 0; i < nlogs_; i++)
    if (ulog_[i].txid_ != 0) {
      auto &log = ulog_[i];
      dump_chunk(log);
    }
}
