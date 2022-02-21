#include "csr_delta.hpp"


void delta_store::initialize() {}

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

    auto iter = nid_to_txid.find(rec.node_id_);
    if (iter == nid_to_txid.end()) {
      // first insertion of neighbour ids associated with nid
      {
        auto beg_iter = ids_.begin() + rec.pos_;
        auto end_iter = beg_iter + rec.count_;
        deltas[rec.node_id_].first.insert(deltas[rec.node_id_].first.end(),
          std::make_move_iterator(beg_iter), std::make_move_iterator(end_iter));
      }
      {
        auto beg_iter = weights_.begin() + rec.pos_;
        auto end_iter = beg_iter + rec.count_;
        deltas[rec.node_id_].second.insert(deltas[rec.node_id_].second.end(),
          std::make_move_iterator(beg_iter), std::make_move_iterator(end_iter));
      }
      nid_to_txid[rec.node_id_] = rec.txid_;
      rec.merged_ = true;
    }
    else if (iter->second < rec.txid_) {
      // delta record is from a newer txn
      // overwrite the vector of neighbour ids with the more recent updates
      deltas[rec.node_id_].first.clear();
      {
        auto beg_iter = ids_.begin() + rec.pos_;
        auto end_iter = beg_iter + rec.count_;
        deltas[rec.node_id_].first.insert(deltas[rec.node_id_].first.end(),
          std::make_move_iterator(beg_iter), std::make_move_iterator(end_iter));
      }
      deltas[rec.node_id_].second.clear();
      {
        auto beg_iter = weights_.begin() + rec.pos_;
        auto end_iter = beg_iter + rec.count_;
        deltas[rec.node_id_].second.insert(deltas[rec.node_id_].second.end(),
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