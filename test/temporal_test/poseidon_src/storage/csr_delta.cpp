#include "csr_delta.hpp"


void delta_store::initialize() {}

#ifdef DIFF_DELTA
void delta_store::store_deltas(uint64_t txid, const transaction::delta_ids &txn_delta_ids) {

  for (auto nid : txn_delta_ids.deleted_nodes_) {
    delta_rec rec(txid, nid, true);
    delta_recs_.store(std::move(rec));
    num_delta_recs_++;
  }
  std::map<offset_t, std::pair<offset_t, delta_rec *>> nid_delta_rec_map;

  for (auto &[nid, deletes] : txn_delta_ids.deleted_neighbours_) {
    delta_rec rec(txid, nid, false);
    rec.deletes_pos_ = next_deletes_pos_;
    rec.deletes_count_ = deletes.size();
    next_deletes_pos_ += deletes.size();
    deletes_.insert(deletes_.end(),
      std::make_move_iterator(deletes.begin()),
        std::make_move_iterator(deletes.end()));
    nid_delta_rec_map.emplace(std::piecewise_construct, std::forward_as_tuple(nid),
                            std::forward_as_tuple(delta_recs_.store(std::move(rec))));
    num_delta_recs_++;
  }

  for (auto &[nid, inserts] : txn_delta_ids.inserted_neighbours_) {
    auto itr = nid_delta_rec_map.find(nid);
    if (itr != nid_delta_rec_map.end()) {
      auto &rec = *(itr->second.second);
      rec.inserts_pos_ = next_inserts_pos_;
      rec.inserts_count_ = inserts.first.size();
      next_inserts_pos_ += inserts.first.size();
      inserts_.insert(inserts_.end(),
        std::make_move_iterator(inserts.first.begin()),
          std::make_move_iterator(inserts.first.end()));
      weights_.insert(weights_.end(),
        std::make_move_iterator(inserts.second.begin()),
          std::make_move_iterator(inserts.second.end()));
    }
    else {
      delta_rec rec(txid, nid, false);
      rec.inserts_pos_ = next_inserts_pos_;
      rec.inserts_count_ = inserts.first.size();
      next_inserts_pos_ += inserts.first.size();
      inserts_.insert(inserts_.end(),
        std::make_move_iterator(inserts.first.begin()),
          std::make_move_iterator(inserts.first.end()));
      weights_.insert(weights_.end(),
        std::make_move_iterator(inserts.second.begin()),
          std::make_move_iterator(inserts.second.end()));

      nid_delta_rec_map.emplace(std::piecewise_construct, std::forward_as_tuple(nid),
                              std::forward_as_tuple(delta_recs_.store(std::move(rec))));
      num_delta_recs_++;
    }
  }
}
#elif defined ADJ_DELTA
void delta_store::store_delta(uint64_t txid, uint64_t nid,
  const std::vector<uint64_t> &ids, const std::vector<double> &weights) {
  delta_rec rec(txid, nid, next_pos_, ids.size());

  next_pos_ += ids.size();
  ids_.insert(ids_.end(),
    std::make_move_iterator(ids.begin()),
      std::make_move_iterator(ids.end()));
  weights_.insert(weights_.end(),
    std::make_move_iterator(weights.begin()),
      std::make_move_iterator(weights.end()));

  delta_recs_.store(std::move(rec));
  num_delta_recs_++;
}
#endif

void delta_store::merge_deltas(delta_map_t &deltas, uint64_t txid) {
  bool clear = true;
  std::map<uint64_t, uint64_t> nid_to_txid;

  for (auto &rec : delta_recs_) {
    if (rec.txid_ > txid) {
      // transaction with id "txid_" started after the one with id "txid" but committed before it
      // therefore, delta records inserted by txid_ should not be visible to txid

      // however, this delta record is still needed later for CSR update
      // therefore, we do not clear the vector of delta records at the end of this merge
      clear = false;
      continue;
    }
    else if (rec.txid_ < last_txn_id_) {
      // delta records inserted by transactions with id < last_txn_id_ would have been included in the
      // previous merge and cleared. If such record still exists in the delta store, then either
      // 1) the record was not cleared after the last merge, or
      // 2) txid_ committed after last_txn_id_

      if (rec.merged_) {
        // 1) the delta record was not cleared after the last merge
        // since the delta record has been merged already, we do not include it in this merge.
        continue;
      }
      else {
        // 2) txid_ committed after last_txn_id_, i.e. txid_ added the delta after the last merge
        // since txid_ has committed, there is no transaction with id > txid_ that added a delta for rec.nid_
        // therefore, we include the delta record in this merge
        ;
      }
    }

#ifdef DIFF_DELTA
    auto iter = nid_to_txid.find(rec.node_id_);
    if (iter == nid_to_txid.end()) {
      // first insertion of delta associated with nid
      // deltas.emplace(std::make_pair(rec.node_id_, delta(rec.node_id_, rec.deleted_)));
      deltas.emplace(std::piecewise_construct,
                     std::forward_as_tuple(rec.node_id_),
                     std::forward_as_tuple(rec.node_id_, rec.deleted_));
      auto &delta = deltas[rec.node_id_];
      if (!delta.deleted_) {
        delta.deletes_.reserve(rec.deletes_count_);
        {
          auto beg_iter = deletes_.begin() + rec.deletes_pos_;
          auto end_iter = beg_iter + rec.deletes_count_;
          delta.deletes_.insert(delta.deletes_.end(),
            std::make_move_iterator(beg_iter), std::make_move_iterator(end_iter));
        }
        delta.inserts_.reserve(rec.inserts_count_);
        {
          auto beg_iter = inserts_.begin() + rec.inserts_pos_;
          auto end_iter = beg_iter + rec.inserts_count_;
          delta.inserts_.insert(delta.inserts_.end(),
            std::make_move_iterator(beg_iter), std::make_move_iterator(end_iter));
        }
        delta.weights_.reserve(rec.inserts_count_);
        {
          auto beg_iter = weights_.begin() + rec.inserts_pos_;
          auto end_iter = beg_iter + rec.inserts_count_;
          delta.weights_.insert(delta.weights_.end(),
            std::make_move_iterator(beg_iter), std::make_move_iterator(end_iter));
        }
      }
      nid_to_txid[rec.node_id_] = rec.txid_;
      rec.merged_ = true;
    }
    else if (iter->second < rec.txid_) {
      // delta element is from a newer txn
      auto &delta = deltas[rec.node_id_];
      if (rec.deleted_) {
        delta.deleted_ = true;
      }
      else {
        // if (delta.deletes_.empty()) {
        delta.deletes_.reserve(delta.deletes_.size() + rec.deletes_count_);
        {
          auto beg_iter = deletes_.begin() + rec.deletes_pos_;
          auto end_iter = beg_iter + rec.deletes_count_;
          delta.deletes_.insert(delta.deletes_.end(),
            std::make_move_iterator(beg_iter), std::make_move_iterator(end_iter));
        }
        delta.inserts_.reserve(delta.inserts_.size() + rec.inserts_count_);
        {
          auto beg_iter = inserts_.begin() + rec.inserts_pos_;
          auto end_iter = beg_iter + rec.inserts_count_;
          delta.inserts_.insert(delta.inserts_.end(),
            std::make_move_iterator(beg_iter), std::make_move_iterator(end_iter));
        }
        delta.weights_.reserve(delta.weights_.size() + rec.inserts_count_);
        {
          auto beg_iter = weights_.begin() + rec.inserts_pos_;
          auto end_iter = beg_iter + rec.inserts_count_;
          delta.weights_.insert(delta.weights_.end(),
            std::make_move_iterator(beg_iter), std::make_move_iterator(end_iter));
        }
      }
      nid_to_txid[rec.node_id_] = rec.txid_;
      rec.merged_ = true;
    }
    else if (iter->second > rec.txid_) {
      // delta element is from an older txn
      auto &delta = deltas[rec.node_id_];
      if (rec.deleted_) {
        delta.deleted_ = true;
      }
      else {
        // if (delta.deletes_.empty()) {
        delta.deletes_.reserve(delta.deletes_.size() + rec.deletes_count_);
        {
          auto beg_iter = deletes_.begin() + rec.deletes_pos_;
          auto end_iter = beg_iter + rec.deletes_count_;
          delta.deletes_.insert(delta.deletes_.end(),
            std::make_move_iterator(beg_iter), std::make_move_iterator(end_iter));
        }
        delta.inserts_.reserve(delta.inserts_.size() + rec.inserts_count_);
        {
          auto beg_iter = inserts_.begin() + rec.inserts_pos_;
          auto end_iter = beg_iter + rec.inserts_count_;
          delta.inserts_.insert(delta.inserts_.end(),
            std::make_move_iterator(beg_iter), std::make_move_iterator(end_iter));
        }
        delta.weights_.reserve(delta.weights_.size() + rec.inserts_count_);
        {
          auto beg_iter = weights_.begin() + rec.inserts_pos_;
          auto end_iter = beg_iter + rec.inserts_count_;
          delta.weights_.insert(delta.weights_.end(),
            std::make_move_iterator(beg_iter), std::make_move_iterator(end_iter));
        }
      }
      rec.merged_ = true;
    }
  }
#elif defined ADJ_DELTA
    auto iter = nid_to_txid.find(rec.node_id_);
    if (iter == nid_to_txid.end()) {
      // first insertion of neighbour ids associated with nid
      deltas[rec.node_id_].node_id_ = rec.node_id_;
      {
        auto beg_iter = ids_.begin() + rec.pos_;
        auto end_iter = beg_iter + rec.count_;
        deltas[rec.node_id_].ids_.insert(deltas[rec.node_id_].ids_.end(),
          std::make_move_iterator(beg_iter), std::make_move_iterator(end_iter));
      }
      {
        auto beg_iter = weights_.begin() + rec.pos_;
        auto end_iter = beg_iter + rec.count_;
        deltas[rec.node_id_].weights_.insert(deltas[rec.node_id_].weights_.end(),
          std::make_move_iterator(beg_iter), std::make_move_iterator(end_iter));
      }
      nid_to_txid[rec.node_id_] = rec.txid_;
      rec.merged_ = true;
    }
    else if (iter->second < rec.txid_) {
      // delta record is from a newer txn
      // overwrite the vector of neighbour ids with the more recent updates
      deltas[rec.node_id_].ids_.clear();
      {
        auto beg_iter = ids_.begin() + rec.pos_;
        auto end_iter = beg_iter + rec.count_;
        deltas[rec.node_id_].ids_.insert(deltas[rec.node_id_].ids_.end(),
          std::make_move_iterator(beg_iter), std::make_move_iterator(end_iter));
      }
      deltas[rec.node_id_].weights_.clear();
      {
        auto beg_iter = weights_.begin() + rec.pos_;
        auto end_iter = beg_iter + rec.count_;
        deltas[rec.node_id_].weights_.insert(deltas[rec.node_id_].weights_.end(),
          std::make_move_iterator(beg_iter), std::make_move_iterator(end_iter));
      }
      nid_to_txid[rec.node_id_] = rec.txid_;
      rec.merged_ = true;
    }
    else if (iter->second > rec.txid_) {
      // delta record is from an older txn
      // do nothing
      ;
    }
  }
#endif

  if (clear) {
    // no delta record is needed later for CSR update
    clear_deltas();
  }
}

void delta_store::clear_deltas() {
  delta_recs_.clear();
  num_delta_recs_ = 0;
  delta_mode_ = true;
}