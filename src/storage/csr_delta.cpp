#include "csr_delta.hpp"


void csr_delta::initialize() {}

void csr_delta::store_delta(uint64_t nid, const std::list<uint64_t> &ids,
                            const std::list<double> &weights, uint64_t txid) {

  auto inserter = [&](delta_element &elem, 
        delta_element::element_type type, uint64_t val) {
    elem.txid_ = txid;
    elem.node_id_ = nid;
    elem.type_ = type;
    elem.val_ = val;
    elem.restored_ = false;

    delta_elements_.store(std::move(elem));
  };

  delta_element nid_elem;
  inserter(nid_elem, delta_element::element_type::node_id, nid);

  for (auto id : ids) {
    delta_element ngbr_id_elem;
    inserter(ngbr_id_elem, delta_element::element_type::neighbour_id, id);
  }

  for (auto w : weights) {
    uint64_t buf;
    std::memcpy(&buf, &w, sizeof(double));

    delta_element rweight_elem;
    inserter(rweight_elem, delta_element::element_type::rship_weight, buf);
  }
}

void csr_delta::restore_deltas(delta_map_t &deltas, uint64_t txid) {
  bool clear = true;
  std::map<uint64_t, uint64_t> nid_to_txid;

  for (auto &elem : delta_elements_) {
    if (elem.txid_ > txid) {
      // txid_ started after txid but committed before it
      // therefore, delta elements inserted by txid_ should not be visible to txid
      
      // however, this delta element is needed for later restores 
      // therefore, we do not clear the vector of delta elements at the end of this restore 
      clear = false;
      continue;
    }
    else if (elem.txid_ < last_txn_id_) {
      // delta elements inserted by transactions with id < last_txn_id_ would have been included in the 
      // previous restore and cleared. If such element still exists in the delta store, then either
      // 1) the element was not cleared after the last restore, or
      // 2) txid_ committed after last_txn_id_
      
      if (elem.restored_) {
        // 1) the delta element was not cleared after the last restore 
        // since the delta element has been used already, we do not include it in this restore.
        continue;
      }
      else {
        // 2) txid_ committed after last_txn_id_, i.e. txid_ added the delta after the last restore 
        // since txid_ has committed, there is no transaction with id > txid_ that added a delta for elem.nid_
        // therefore, we include the delta element in this restore
        ;
      }
    }

    if (elem.type_ == delta_element::element_type::node_id) {
      deltas[elem.node_id_];
      elem.restored_ = true;
    }
    else if (elem.type_ == delta_element::element_type::neighbour_id) {
      auto iter = nid_to_txid.find(elem.node_id_);
      if (iter == nid_to_txid.end()) {
        // first insertion of neighbour ids associated with nid
        nid_to_txid[elem.node_id_] = elem.txid_;
        deltas[elem.node_id_].first.push_back(elem.val_);
        elem.restored_ = true;
      }
      else if (iter->second == elem.txid_) {
        // delta element is from the same txn
        deltas[elem.node_id_].first.push_back(elem.val_);
        elem.restored_ = true;
      }
      else if (iter->second < elem.txid_) {
        // delta element is from a newer txn
        // overwrite the vector of neighbour ids with the more recent updates
        deltas[elem.node_id_].first.clear();
        nid_to_txid[elem.node_id_] = elem.txid_;
        deltas[elem.node_id_].first.push_back(elem.val_);
        elem.restored_ = true;
      }
      else if (iter->second > elem.txid_) {
        // delta element is from an older txn 
        // do nothing
        ;
      }
    }
    else if (elem.type_ == delta_element::element_type::rship_weight) {
      double buf;
      std::memcpy(&buf, &elem.val_, sizeof(buf));

      auto iter = nid_to_txid.find(elem.node_id_);
      if (iter == nid_to_txid.end()) {
        // first insertion of relationship weights associated with nid
        nid_to_txid[elem.node_id_] = elem.txid_;
        deltas[elem.node_id_].second.push_back(buf);
        elem.restored_ = true;
      }
      else if (iter->second == elem.txid_) {
        // delta element is from the same txn
        deltas[elem.node_id_].second.push_back(buf);
        elem.restored_ = true;
      }
      else if (iter->second < elem.txid_) {
        // delta element is from a newer txn
        // overwrite the vector of neighbour ids with the more recent updates
        deltas[elem.node_id_].second.clear();
        nid_to_txid[elem.node_id_] = elem.txid_;
        deltas[elem.node_id_].second.push_back(buf);
        elem.restored_ = true;
      }
      else if (iter->second > elem.txid_) {
        // delta element is from an older txn 
        // do nothing
        ;
      }
    }
  }
  if (clear) {
    // no delta element is needed for later restores
    delta_elements_.clear();
  }
}