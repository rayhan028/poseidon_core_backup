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

/**
 * pmlog implements a undo log for transaction processing storing the log entries in persistent
 * memory. For each active transaction a persistent list of log entries is maintained which can
 * be traversed during recovery.
 */
class pmlog {
  struct log_chunk {
    uint8_t data_[4076];
    xid_t txid_;
    uint32_t used_;
    p_ptr<log_chunk> next_;
  };

  using chunk_ptr = p_ptr<log_chunk>;

public:
  using id_t = std::size_t;

  /**
   * The different kinds of log entries: insert, update, delete.
   */
  enum log_entry_type { log_insert = 1, log_update = 2, log_delete = 3 };

  /**
   * The different objects (nodes, relationships, property_set) represented
   * by the log entries.
   */
  enum log_object_type { log_node = 1, log_rship = 2, log_property = 3 };

  /**
   * An iterator for traversing the log entries.
   */
  struct log_rec_iter {
#ifdef USE_PMDK
    log_rec_iter(p_ptr<log_chunk> ptr, std::size_t p = 0) : 
        chunk_(ptr), pos_(p) {}
#else
    log_rec_iter(log_chunk *ptr, std::size_t p = 0) : chunk_(ptr), pos_(p) {}
#endif
    bool operator!=(const log_rec_iter &other) const {
      return chunk_ != other.chunk_ || pos_ != other.pos_;
    }

    log_rec_iter &operator++();

    log_entry_type log_type() const;
    log_object_type obj_type() const;

    template <typename T> T *get() { return (T *)(&(chunk_->data_[pos_])); }

#ifdef USE_PMDK
    p_ptr<log_chunk> chunk_;
#else
    log_chunk *chunk_;
#endif
    std::size_t pos_;
  };

  /**
   * An iterator for traversing the different logs (per transaction).
   */
  class log_iter {
  public:
#ifdef USE_PMDK
    log_iter(p_ptr<log_chunk[]> l, std::size_t m, std::size_t p = 0)
        : log_(l), cpos_(p), maxpos_(m) {}
#else
    log_iter(log_chunk *l, std::size_t m, std::size_t p = 0)
        : log_(l), cpos_(p), maxpos_(m) {}
#endif
    bool operator!=(const log_iter &other) const {
      return log_ != other.log_ || cpos_ != other.cpos_;
    }
    log_iter &operator++() {
      cpos_++;
      return *this;
    }

    /**
     * Returns true if this log contains log entries.
     */
    bool valid() const { return cpos_ < maxpos_ && log_[cpos_].txid_ != 0; }

    /**
     * Returns the transaction id for which this log was stored.
     */
    xid_t txid() const { return log_[cpos_].txid_; }

    log_rec_iter begin() { return log_rec_iter(&(log_[cpos_])); }
    log_rec_iter end() {
      return log_rec_iter(&(log_[cpos_]),
                          std::numeric_limits<std::size_t>::max());
    }

  private:
#ifdef USE_PMDK
    p_ptr<log_chunk[]> log_;
#else
    log_chunk *log_;
#endif
    std::size_t cpos_, maxpos_;
  };

  /**
   * Construct a new and empty log.
   */
  pmlog();

  /**
   * Destructor
   */
  ~pmlog();

  /**
   * This method should be called when a new transaction starts. It reserves a new log and 
   * returns its id which should be used for this transaction.
   */
  id_t transaction_begin(xid_t txid);

  /**
   * This method is called after the transaction was finished (commit or abort). It cleans up
   * the log, i.e. removes all undo entries.
   */
  void transaction_end(id_t log_id);

  /**
   * Append a log entry to the given log. log_entry is the address of a log record as defined above.
   */
  void append(id_t log_id, void *log_entry, uint32_t lsize);

  /**
   * Iteratos for traversing the logs.
   */
  log_iter log_begin() { return log_iter(ulog_, nlogs_); }
  log_iter log_end() { return log_iter(ulog_, nlogs_, nlogs_); }

  void dump();

private:
  void dump_chunk(log_chunk &log);

#ifdef USE_PMDK
  p_ptr<log_chunk[]> ulog_;
#else
  log_chunk *ulog_;
#endif
  p<std::size_t> nlogs_;
};

#endif
