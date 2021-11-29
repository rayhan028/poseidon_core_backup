#include "csr_delta.hpp"


void csr_delta::initialize() {
}

void csr_delta::add_update_delta(uint64_t nid, std::vector<uint64_t> &&ids,
                                 std::vector<double> &&weights) {
  // TODO handle multiple updated states of same nid
  update_deltas_.store({nid, delta_element::element_type::node_id, nid});

  for (auto id : ids)
    update_deltas_.store({nid, delta_element::element_type::neighbour_id, id});

  for (auto w : weights) {
    uint64_t buf;
    std::memcpy(&buf, &w, sizeof(double));
    update_deltas_.store({nid, delta_element::element_type::rship_weight, buf});
  }
}

void csr_delta::add_append_delta(uint64_t nid, std::vector<uint64_t> &&ids,
                                 std::vector<double> &&weights) {
  append_deltas_.store({nid, delta_element::element_type::node_id, nid});

  for (auto id : ids)
    append_deltas_.store({nid, delta_element::element_type::neighbour_id, id});

  for (auto w : weights) {
    uint64_t buf;
    std::memcpy(&buf, &w, sizeof(double));
    append_deltas_.store({nid, delta_element::element_type::rship_weight, buf});
  }
}

void csr_delta::restore_deltas(delta_map_t &update_delta_map,
                               delta_map_t &append_delta_map) {
  // TODO handle uni-directional restore when store is bidirectional
  for (auto &elem : update_deltas_) {
    if (elem.type_ == delta_element::element_type::node_id) {
      update_delta_map[elem.node_id_];
    } else if (elem.type_ == delta_element::element_type::neighbour_id) {
      update_delta_map[elem.node_id_].first.push_back(elem.val_);
    } else if (elem.type_ == delta_element::element_type::rship_weight) {
      double d;
      std::memcpy(&d, &elem.val_, sizeof(d));
      update_delta_map[elem.node_id_].second.push_back(d);
    }
  }

  for (auto &elem : append_deltas_) {
    if (elem.type_ == delta_element::element_type::node_id) {
      append_delta_map[elem.node_id_];
    } else if (elem.type_ == delta_element::element_type::neighbour_id) {
      append_delta_map[elem.node_id_].first.push_back(elem.val_);
    } else if (elem.type_ == delta_element::element_type::rship_weight) {
      double d;
      std::memcpy(&d, &elem.val_, sizeof(d));
      append_delta_map[elem.node_id_].second.push_back(d);
    }
  }
}