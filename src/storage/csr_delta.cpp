#include "csr_delta.hpp"


csr_delta::csr_delta() {
  auto pop = pmem::obj::pool_by_vptr(this);
  pmem::obj::transaction::run(pop, [&] {
      update_deltas_ = pmem::obj::make_persistent<delta_map_t>();
      append_deltas_ = pmem::obj::make_persistent<delta_map_t>();
  });
}

csr_delta::~csr_delta() {
  auto pop = pmem::obj::pool_by_vptr(this);
  pmem::obj::transaction::run(pop, [&] {
      pmem::obj::delete_persistent<delta_map_t>(update_deltas_);
      pmem::obj::delete_persistent<delta_map_t>(append_deltas_);
  });
}

void csr_delta::initialize() {
  update_deltas_->runtime_initialize();
  append_deltas_->runtime_initialize();
}

void csr_delta::add_update_delta(uint64_t nid, const std::vector<uint64_t> &ids,
                                    const std::vector<double> &weights) {
  p_ptr<id_vector_t> id_vec;
  p_ptr<weight_vector_t> weight_vec;
  p_ptr<vector_pair_t> vec_pair;

  auto pop = pmem::obj::pool_by_vptr(this);
  pmem::obj::transaction::run(pop, [&] {
    id_vec = p_make_ptr<id_vector_t>(ids);
    weight_vec = p_make_ptr<weight_vector_t>(weights);
  });

  pmem::obj::transaction::run(pop, [&] {
    vec_pair = p_make_ptr<vector_pair_t>(id_vec, weight_vec);
  });

  /*delta_map_t::accessor acc;
  update_deltas_->insert(acc, nid);
  pmem::obj::transaction::run(pop, [&] {
    acc->second = p_make_ptr<vector_pair_t>(id_vec, weight_vec);
  });
  acc.release();*/

  update_deltas_->insert(delta_map_val_t(nid, vec_pair));
}

void csr_delta::add_append_delta(uint64_t nid, const std::vector<uint64_t> &ids,
                                    const std::vector<double> &weights) {
  p_ptr<id_vector_t> id_vec;
  p_ptr<weight_vector_t> weight_vec;
  p_ptr<vector_pair_t> vec_pair;

  auto pop = pmem::obj::pool_by_vptr(this);
  pmem::obj::transaction::run(pop, [&] {
    id_vec = p_make_ptr<id_vector_t>(ids);
    weight_vec = p_make_ptr<weight_vector_t>(weights);
  });

  pmem::obj::transaction::run(pop, [&] {
    vec_pair = p_make_ptr<vector_pair_t>(id_vec, weight_vec);
  });

  /*delta_map_t::accessor acc;
  append_deltas_->insert(acc, nid);
  pmem::obj::transaction::run(pop, [&] {
    acc->second = p_make_ptr<vector_pair_t>(id_vec, weight_vec);
  });
  acc.release();*/

  append_deltas_->insert(delta_map_val_t(nid, vec_pair));
}