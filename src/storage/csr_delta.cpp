#include "csr_delta.hpp"


void csr_delta::initialize() {
}

void csr_delta::add_update_delta(uint64_t txid, uint64_t nid, const std::list<uint64_t> &ids,
                                 const std::list<double> &weights) {
  delta_element nid_elem;
  nid_elem.txid_ = txid;
  nid_elem.node_id_ = nid;
  nid_elem.type_ = delta_element::element_type::node_id;
  nid_elem.val_ = nid;

  update_deltas_.store(std::move(nid_elem));

  for (auto id : ids) {
    delta_element ngbr_id_elem;
    ngbr_id_elem.txid_ = txid;
    ngbr_id_elem.node_id_ = nid;
    ngbr_id_elem.type_ = delta_element::element_type::neighbour_id;
    ngbr_id_elem.val_ = id;

    update_deltas_.store(std::move(ngbr_id_elem));
  }

  for (auto w : weights) {
    uint64_t buf;
    std::memcpy(&buf, &w, sizeof(double));

    delta_element rweight_elem;
    rweight_elem.txid_ = txid;
    rweight_elem.node_id_ = nid;
    rweight_elem.type_ = delta_element::element_type::rship_weight;
    rweight_elem.val_ = buf;

    update_deltas_.store(std::move(rweight_elem));
  }
}

void csr_delta::add_append_delta(uint64_t txid, uint64_t nid, const std::list<uint64_t> &ids,
                                 const std::list<double> &weights) {
  delta_element nid_elem;
  nid_elem.txid_ = txid;
  nid_elem.node_id_ = nid;
  nid_elem.type_ = delta_element::element_type::node_id;
  nid_elem.val_ = nid;

  append_deltas_.store(std::move(nid_elem));

  for (auto id : ids) {
    delta_element ngbr_id_elem;
    ngbr_id_elem.txid_ = txid;
    ngbr_id_elem.node_id_ = nid;
    ngbr_id_elem.type_ = delta_element::element_type::neighbour_id;
    ngbr_id_elem.val_ = id;

    append_deltas_.store(std::move(ngbr_id_elem));
  }

  for (auto w : weights) {
    uint64_t buf;
    std::memcpy(&buf, &w, sizeof(double));

    delta_element rweight_elem;
    rweight_elem.txid_ = txid;
    rweight_elem.node_id_ = nid;
    rweight_elem.type_ = delta_element::element_type::rship_weight;
    rweight_elem.val_ = buf;

    append_deltas_.store(std::move(rweight_elem));
  }
}

void csr_delta::restore_deltas(delta_map_t &&update_delta_map,
                               delta_map_t &&append_delta_map,
                               offset_t last_id) {
  std::map<uint64_t, uint64_t> nid_to_txid;
  for (auto &elem : update_deltas_) {
    if (elem.type_ == delta_element::element_type::node_id) {
      update_delta_map[elem.node_id_];
    }
    else if (elem.type_ == delta_element::element_type::neighbour_id) {
      auto iter = nid_to_txid.find(elem.node_id_);
      if (iter == nid_to_txid.end()) {
        // first insertion
        nid_to_txid[elem.node_id_] = elem.txid_;
        update_delta_map[elem.node_id_].first.push_back(elem.val_);
      }
      else if (iter->second == elem.txid_) {
        // delta element is from the same txn
        update_delta_map[elem.node_id_].first.push_back(elem.val_);
      }
      else if (iter->second < elem.txid_) {
        // delta element is from a newer txn
        // overwrite the vector of neighbour ids with the more recent updates
        update_delta_map[elem.node_id_].first.clear();
        nid_to_txid[elem.node_id_] = elem.txid_;
        update_delta_map[elem.node_id_].first.push_back(elem.val_);
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
        // first insertion
        nid_to_txid[elem.node_id_] = elem.txid_;
        update_delta_map[elem.node_id_].second.push_back(buf);
      }
      else if (iter->second == elem.txid_) {
        // delta element is from the same txn
        update_delta_map[elem.node_id_].second.push_back(buf);
      }
      else if (iter->second < elem.txid_) {
        // delta element is from a newer txn
        // overwrite the vector of neighbour ids with the more recent updates
        update_delta_map[elem.node_id_].first.clear();
        nid_to_txid[elem.node_id_] = elem.txid_;
        update_delta_map[elem.node_id_].second.push_back(buf);
      }
      else if (iter->second > elem.txid_) {
        // delta element is from an older txn 
        // do nothing
        ;
      }
    }
  }

  for (auto &elem : append_deltas_) {
    if (elem.type_ == delta_element::element_type::node_id) {
      append_delta_map[elem.node_id_];
    } else if (elem.type_ == delta_element::element_type::neighbour_id) {
      auto iter = nid_to_txid.find(elem.node_id_);
      if (iter == nid_to_txid.end()) {
        // first insertion
        nid_to_txid[elem.node_id_] = elem.txid_;
        append_delta_map[elem.node_id_].first.push_back(elem.val_);
      }
      else if (iter->second == elem.txid_) {
        // delta element is from the same txn
        append_delta_map[elem.node_id_].first.push_back(elem.val_);
      }
      else if (iter->second < elem.txid_) {
        // delta element is from a newer txn
        // overwrite the vector of neighbour ids with the more recent updates
        append_delta_map[elem.node_id_].first.clear();
        nid_to_txid[elem.node_id_] = elem.txid_;
        append_delta_map[elem.node_id_].first.push_back(elem.val_);
      }
      else if (iter->second > elem.txid_) {
        // delta element is from an older txn 
        // do nothing
        ;
      }
    } else if (elem.type_ == delta_element::element_type::rship_weight) {
      double buf;
      std::memcpy(&buf, &elem.val_, sizeof(buf));

      auto iter = nid_to_txid.find(elem.node_id_);
      if (iter == nid_to_txid.end()) {
        // first insertion
        nid_to_txid[elem.node_id_] = elem.txid_;
        append_delta_map[elem.node_id_].second.push_back(buf);
      }
      else if (iter->second == elem.txid_) {
        // delta element is from the same txn
        append_delta_map[elem.node_id_].second.push_back(buf);
      }
      else if (iter->second < elem.txid_) {
        // delta element is from a newer txn
        // overwrite the vector of neighbour ids with the more recent updates
        append_delta_map[elem.node_id_].first.clear();
        nid_to_txid[elem.node_id_] = elem.txid_;
        append_delta_map[elem.node_id_].second.push_back(buf);
      }
      else if (iter->second > elem.txid_) {
        // delta element is from an older txn 
        // do nothing
        ;
      }
    }
  }
  // clear delta lists
  update_deltas_.clear();
  append_deltas_.clear();

  // update last id
  last_node_id_ = last_id;
}

void csr_delta::reset_csr_delta(bool bidir, rship_weight func, offset_t node_id) {
  update_deltas_.clear();
  append_deltas_.clear();

  bidirectional_ = bidir;
  weight_func_ = func;
  last_node_id_ = node_id;
}