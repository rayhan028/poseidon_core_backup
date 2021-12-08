#include "csr_delta.hpp"


void csr_delta::initialize() {
}

void csr_delta::add_update_delta(uint64_t nid, std::vector<uint64_t> &&ids,
                                 std::vector<double> &&weights) {
  // TODO handle multiple updated states of same nid
  delta_element nid_elem;
  nid_elem.node_id_ = nid;
  nid_elem.type_ = delta_element::element_type::node_id;
  nid_elem.val_ = nid;

  update_deltas_.store(std::move(nid_elem));

  for (auto id : ids) {
    delta_element ngbr_id_elem;
    ngbr_id_elem.node_id_ = nid;
    ngbr_id_elem.type_ = delta_element::element_type::neighbour_id;
    ngbr_id_elem.val_ = id;

    update_deltas_.store(std::move(ngbr_id_elem));
  }

  for (auto w : weights) {
    uint64_t buf;
    std::memcpy(&buf, &w, sizeof(double));

    delta_element rweight_elem;
    rweight_elem.node_id_ = nid;
    rweight_elem.type_ = delta_element::element_type::rship_weight;
    rweight_elem.val_ = buf;

    update_deltas_.store(std::move(rweight_elem));
  }
}

void csr_delta::add_append_delta(uint64_t nid, std::vector<uint64_t> &&ids,
                                 std::vector<double> &&weights) {
  delta_element nid_elem;
  nid_elem.node_id_ = nid;
  nid_elem.type_ = delta_element::element_type::node_id;
  nid_elem.val_ = nid;

  append_deltas_.store(std::move(nid_elem));

  for (auto id : ids) {
    delta_element ngbr_id_elem;
    ngbr_id_elem.node_id_ = nid;
    ngbr_id_elem.type_ = delta_element::element_type::neighbour_id;
    ngbr_id_elem.val_ = id;

    append_deltas_.store(std::move(ngbr_id_elem));
  }

  for (auto w : weights) {
    uint64_t buf;
    std::memcpy(&buf, &w, sizeof(double));

    delta_element rweight_elem;
    rweight_elem.node_id_ = nid;
    rweight_elem.type_ = delta_element::element_type::rship_weight;
    rweight_elem.val_ = buf;

    append_deltas_.store(std::move(rweight_elem));
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