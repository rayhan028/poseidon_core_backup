#include "query_ctx.hpp"
#include "thread_pool.hpp"

scan_task::scan_task(graph_db_ptr gdb, std::size_t first, std::size_t last, query_ctx::node_consumer_func c, transaction_ptr tp, std::size_t start_pos)
	: graph_db_(gdb), range_(first, last), consumer_(c), tx_(tp), start_pos_(start_pos) {}

void scan_task::scan(transaction_ptr tx, graph_db_ptr gdb, std::size_t first, std::size_t last, query_ctx::node_consumer_func consumer) {
    xid_t xid = 0;
    if (tx) { // we need the transaction pointer in thread-local storage
	    current_transaction_ = tx;
	    xid = tx->xid();				    
    }
    auto iter = gdb->get_nodes()->range(first, last);
    while (iter) {
#ifdef USE_TX
	    auto &n = *iter;
	    if (n.is_valid()) {
	      auto &nv = gdb->get_valid_node_version(n, xid);
		    consumer(nv);
#ifdef QOP_RECOVERY
        gdb->store_iter({iter.get_cur_chunk(), iter.get_cur_pos()});
#endif
	  }
#else
	  consumer_(*iter);
#endif
	  ++iter;
  }
}

std::function<void(transaction_ptr tx, graph_db_ptr gdb, std::size_t first, std::size_t last, query_ctx::node_consumer_func consumer)> scan_task::callee_ = &scan_task::scan;

void scan_task::operator()() {
   callee_(tx_, graph_db_, range_.first, range_.second, consumer_);
}

query_ctx::~query_ctx() {
}
 
void query_ctx::_nodes_by_label(graph_db *gdb, const std::string &label,
                              node_consumer_func consumer) {
#ifdef USE_TX
  check_tx_context();
  xid_t txid = current_transaction()->xid();
#endif
  auto lc = gdb->dict_->lookup_string(label);
  // spdlog::info("_nodes_by_label: '{}' -> {}", label, lc);
  for (auto &n : gdb->nodes_->as_vec()) {

#ifdef USE_TX
    if (n.is_valid()) {
      auto &nv = gdb->get_valid_node_version(n, txid);
      if (nv.node_label == lc) {
        consumer(nv);
      }
    }
#else
    if (n.node_label == lc)
      consumer(n);
#endif
  }
}

void query_ctx::nodes_by_label(const std::string &label,
                              node_consumer_func consumer) {
    _nodes_by_label(gdb_.get(), label, consumer);
/*
#ifdef USE_TX
  check_tx_context();
  xid_t txid = current_transaction()->xid();
#endif
  auto lc = gdb_->dict_->lookup_string(label);
  for (auto &n : gdb_->nodes_->as_vec()) {
#ifdef USE_TX
    if (n.is_valid()) {
      auto &nv = gdb_->get_valid_node_version(n, txid);
      if (nv.node_label == lc) {
        consumer(nv);
      }
    }
#else
    if (n.node_label == lc)
      consumer(n);
#endif
  }
  */
}

void query_ctx::nodes_by_label(const std::vector<std::string> &labels,
                              node_consumer_func consumer) {
#ifdef USE_TX
  check_tx_context();
  xid_t txid = current_transaction()->xid();
#endif
  std::vector<dcode_t> codes(labels.size());
  for (auto i = 0u; i < labels.size(); i++) 
    codes[i] = gdb_->dict_->lookup_string(labels[i]);
  for (auto &n : gdb_->nodes_->as_vec()) {
#ifdef USE_TX
    if (n.is_valid()) {
      auto &nv = gdb_->get_valid_node_version(n, txid);
      for (auto &lc : codes) {
        if (nv.node_label == lc) {
          consumer(nv);
          break;
        }
      }
    }
#else
    for (auto &lc : codes) {
      if (n.node_label == lc) {
        consumer(n);
        break;
      }
    }
#endif
  }
}

void query_ctx::parallel_nodes(node_consumer_func consumer) {
#ifdef USE_TX
  check_tx_context();
#endif
  std::vector<std::future<void>> res;
  thread_pool pool;

  const int nchunks = 1;
  spdlog::debug("Start parallel query with {} threads",
                gdb_->nodes_->num_chunks() / nchunks + 1);

  res.reserve(gdb_->nodes_->num_chunks() / nchunks + 1);
  std::size_t start = 0, end = nchunks - 1;
  while (start < gdb_->nodes_->num_chunks()) {
    res.push_back(pool.submit(
        scan_task(gdb_, start, end, consumer, current_transaction_)));
    start = end + 1;
    end += nchunks;
  }
 
  // std::cout << "waiting ..." << std::endl;
  for (auto &f : res) {
    f.get();
  }
}
#ifdef QOP_RECOVERY
void query_ctx::parallel_nodes(node_consumer_func consumer, std::map<std::size_t, std::vector<std::size_t>> &range_map) {
#ifdef USE_TX
  check_tx_context();
#endif
  const int nchunks = 25;
  spdlog::debug("Start parallel query with {} threads",
                range_map.size());

  std::vector<std::future<void>> res;
  res.reserve(range_map.size());
  thread_pool pool;

  for(auto & r : range_map) {
    auto start = r.second.front();
    auto end = r.second.back();
    res.push_back(pool.submit(
        scan_task(this, *nodes_, start, end, consumer, current_transaction_)));
  }
  
  // std::cout << "waiting ..." << std::endl;
  for (auto &f : res) {
    f.get();
  }
}
#endif 

void query_ctx::nodes(node_consumer_func consumer) {
#ifdef USE_TX
  check_tx_context();
  xid_t txid = current_transaction()->xid();
#endif

  for (auto &n : gdb_->nodes_->as_vec()) {
#ifdef USE_TX
    // spdlog::info("#{} ===> {},{} | {}", n.id(), short_ts(n.bts), short_ts(n.cts), short_ts(txid));
    if (n.is_valid()) {
      try {
        auto &nv = gdb_->get_valid_node_version(n, txid);
        consumer(nv);
      } catch (unknown_id& exc) { /* ignore */ }
    }
#else
    consumer(n);
#endif
  }
}

void query_ctx::nodes_where(const std::string &pkey, p_item::predicate_func pred,
                           node_consumer_func consumer) {
  auto pc = gdb_->dict_->lookup_string(pkey);
  gdb_->node_properties_->foreach(pc, pred, [&](offset_t nid) {
    auto &n = gdb_->node_by_id(nid);
    consumer(n);
  });
}

void query_ctx::relationships_by_label(const std::string &label,
                                      rship_consumer_func consumer) {
#ifdef USE_TX
  check_tx_context();
  xid_t txid = current_transaction()->xid();
#endif

  auto lc = gdb_->dict_->lookup_string(label);
  for (auto &r : gdb_->rships_->as_vec()) {
#ifdef USE_TX
    auto &rv = gdb_->get_valid_rship_version(r, txid);
    if (rv.rship_label == lc)
      consumer(rv);
#else
    if (r.rship_label == lc)
      consumer(r);
#endif
  }
}

void query_ctx::foreach_from_relationship_of_node(const node &n,
                                                 rship_consumer_func consumer) {
  auto relship_id = n.from_rship_list;
  while (relship_id != UNKNOWN) {
    auto &relship = gdb_->rship_by_id(relship_id);
    if (relship.is_valid())
      consumer(relship);
    relship_id = relship.next_src_rship;
  }
}

void query_ctx::foreach_variable_from_relationship_of_node(
    const node &n, std::size_t min, std::size_t max,
    rship_consumer_func consumer) {
  std::list<std::pair<relationship::id_t, std::size_t>> rship_queue;
  rship_queue.push_back(std::make_pair(n.from_rship_list, 1));

  while (!rship_queue.empty()) {
    auto p = rship_queue.front();
    rship_queue.pop_front();
    auto relship_id = p.first;
    auto hops = p.second;
    if (relship_id == UNKNOWN || hops > max)
      continue;

    auto &relship = gdb_->rship_by_id(relship_id);

    if (hops >= min)
      consumer(relship);

    // scan recursively!!
    rship_queue.push_back(std::make_pair(relship.next_src_rship, hops));

    auto &dest = gdb_->node_by_id(relship.dest_node);
    rship_queue.push_back(std::make_pair(dest.from_rship_list, hops + 1));
  }
}

void query_ctx::foreach_variable_from_relationship_of_node(
    const node &n, dcode_t lcode, std::size_t min, std::size_t max,
    rship_consumer_func consumer) {
  std::set<relationship::id_t> rship_set;
  std::list<std::pair<relationship::id_t, std::size_t>> rship_queue;
  rship_queue.push_back(std::make_pair(n.from_rship_list, 1));

  // keep track of potential n-hop rship matches starting 
  // with 1-hop rship matches (n = 1)

  // count all potential 1-hop rship matches
  auto n_hop_rship_cnt = 0;
  auto n_hop_rship_id = n.from_rship_list; 
  while (n_hop_rship_id != UNKNOWN){
    n_hop_rship_cnt++;
    n_hop_rship_id = gdb_->rship_by_id(n_hop_rship_id).next_src_rship;
  }
  std::size_t mr_n_hop = 1;
  auto mr_n_hop_rship_id = n.from_rship_list;
  
  while (!rship_queue.empty()) {
    auto p = rship_queue.front();
    rship_queue.pop_front();
    auto relship_id = p.first;
    auto hops = p.second;
    
    // keep track of the most recently accessed rship and update count 
    if (hops == mr_n_hop){
      mr_n_hop_rship_id = relship_id;
      n_hop_rship_cnt--;
    }

    if (relship_id == UNKNOWN || hops > max)
      continue;

    auto &relship = gdb_->rship_by_id(relship_id);

    // just about to exit the while loop
    if (rship_queue.empty() && (relship.rship_label != lcode)){
      // check if any potential n-hop rship match still exists 
      if (n_hop_rship_cnt > 0) {
        mr_n_hop_rship_id = gdb_->rship_by_id(mr_n_hop_rship_id).next_src_rship;
        rship_queue.push_back(std::make_pair(mr_n_hop_rship_id, mr_n_hop));
        continue;
      } 
      // recursively check if any potential (n+1)-hop rship exists
      else if (relship.next_src_rship != UNKNOWN){
        rship_queue.push_back(std::make_pair(relship.next_src_rship, hops));

        // keep track of potential (n+1)-hop rship matches 

        // count all potential (n+1)-hop rship matches
        n_hop_rship_cnt = 0;
        n_hop_rship_id = relship.next_src_rship;
        while (n_hop_rship_id != UNKNOWN){
          n_hop_rship_cnt++;
          n_hop_rship_id = gdb_->rship_by_id(n_hop_rship_id).next_src_rship;
        }
        mr_n_hop = hops;
        mr_n_hop_rship_id = relship.next_src_rship;
        continue;
      }
      // finally exit the while loop if no potential rship exists
      else 
        continue;
    }
    
    if (relship.rship_label != lcode)
      continue;

    if (hops >= min) {
      if (rship_set.find(relship.id()) != rship_set.end())
        continue;
      rship_set.insert(relship.id());
      consumer(relship);
    }

    // scan recursively!!
    rship_queue.push_back(std::make_pair(relship.next_src_rship, hops));

    auto &dest = gdb_->node_by_id(relship.dest_node);
    auto path_rship_id = dest.from_rship_list;
    rship_queue.push_back(std::make_pair(path_rship_id, hops + 1));
    // rship_queue might not be empty after path_rship_id is processed
    while (path_rship_id != UNKNOWN){
      path_rship_id = gdb_->rship_by_id(path_rship_id).next_src_rship;
      rship_queue.push_back(std::make_pair(path_rship_id, hops + 1));
    }
  }
}

void query_ctx::foreach_variable_to_relationship_of_node(
    const node &n, std::size_t min, std::size_t max,
    rship_consumer_func consumer) {
  // the queue of relationships to be considered plus the number of hops to
  // them
  std::list<std::pair<relationship::id_t, std::size_t>> rship_queue;
  // initialize the queue with the first relationship
  rship_queue.push_back(std::make_pair(n.to_rship_list, 1));

  // as long we have something to traverse
  while (!rship_queue.empty()) {
    auto p = rship_queue.front();
    rship_queue.pop_front();
    auto relship_id = p.first;
    auto hops = p.second;

    // end of relationships list or too many hops
    if (relship_id == UNKNOWN || hops > max)
      continue;

    auto &relship = gdb_->rship_by_id(relship_id);

    if (hops >= min)
      consumer(relship);

    // we proceed with the next relationship
    rship_queue.push_back(std::make_pair(relship.next_dest_rship, hops));

    // scan recursively: get the node and the first outgoing relationship of
    // this node and add it to the queue
    auto &src = gdb_->node_by_id(relship.src_node);
    rship_queue.push_back(std::make_pair(src.to_rship_list, hops + 1));
  }
}

void query_ctx::foreach_variable_to_relationship_of_node(
    const node &n, dcode_t lcode, std::size_t min, std::size_t max,
    rship_consumer_func consumer) {
  std::set<relationship::id_t> rship_set;
  std::list<std::pair<relationship::id_t, std::size_t>> rship_queue;
  rship_queue.push_back(std::make_pair(n.to_rship_list, 1));

  // keep track of potential n-hop rship matches starting 
  // with 1-hop rship matches (n = 1)

  // count all potential 1-hop rship matches
  auto n_hop_rship_cnt = 0;
  auto n_hop_rship_id = n.to_rship_list; 
  while (n_hop_rship_id != UNKNOWN){
    n_hop_rship_cnt++;
    n_hop_rship_id = gdb_->rship_by_id(n_hop_rship_id).next_dest_rship;
  }
  std::size_t mr_n_hop = 1;
  auto mr_n_hop_rship_id = n.to_rship_list;
  
  while (!rship_queue.empty()) {
    auto p = rship_queue.front();
    rship_queue.pop_front();
    auto relship_id = p.first;
    auto hops = p.second;
    
    // keep track of the most recently accessed rship and update count 
    if (hops == mr_n_hop){
      mr_n_hop_rship_id = relship_id;
      n_hop_rship_cnt--;
    }

    if (relship_id == UNKNOWN || hops > max)
      continue;

    auto &relship = gdb_->rship_by_id(relship_id);

    // just about to exit the while loop
    if (rship_queue.empty() && (relship.rship_label != lcode)){
      // check if any potential n-hop rship match still exists 
      if (n_hop_rship_cnt > 0) {
        mr_n_hop_rship_id = gdb_->rship_by_id(mr_n_hop_rship_id).next_dest_rship;
        rship_queue.push_back(std::make_pair(mr_n_hop_rship_id, mr_n_hop));
        continue;
      } 
      // recursively check if any potential (n+1)-hop rship exists
      else if (relship.next_dest_rship != UNKNOWN){
        rship_queue.push_back(std::make_pair(relship.next_dest_rship, hops));

        // keep track of potential (n+1)-hop rship matches 

        // count all potential (n+1)-hop rship matches
        n_hop_rship_cnt = 0;
        n_hop_rship_id = relship.next_dest_rship;
        while (n_hop_rship_id != UNKNOWN){
          n_hop_rship_cnt++;
          n_hop_rship_id = gdb_->rship_by_id(n_hop_rship_id).next_dest_rship;
        }
        mr_n_hop = hops;
        mr_n_hop_rship_id = relship.next_dest_rship;
        continue;
      }
      // finally exit the while loop if no potential rship exists
      else 
        continue;
    }
    
    if (relship.rship_label != lcode)
      continue;

    if (hops >= min) {
      if (rship_set.find(relship.id()) != rship_set.end())
        continue;
      rship_set.insert(relship.id());
      consumer(relship);
    }

    // scan recursively!!
    rship_queue.push_back(std::make_pair(relship.next_dest_rship, hops));

    auto &src = gdb_->node_by_id(relship.src_node);
    auto path_rship_id = src.to_rship_list;
    rship_queue.push_back(std::make_pair(path_rship_id, hops + 1));
    // rship_queue might not be empty after path_rship_id is processed
    while (path_rship_id != UNKNOWN){
      path_rship_id = gdb_->rship_by_id(path_rship_id).next_dest_rship;
      rship_queue.push_back(std::make_pair(path_rship_id, hops + 1));
    }
  }
}

void query_ctx::foreach_from_relationship_of_node(const node &n,
                                                 const std::string &label,
                                                 rship_consumer_func consumer) {
  auto lc = gdb_->dict_->lookup_string(label);
  foreach_from_relationship_of_node(n, lc, consumer);
}

void query_ctx::foreach_from_relationship_of_node(const node &n, dcode_t lcode,
                                                 rship_consumer_func consumer) {
  auto relship_id = n.from_rship_list;
  while (relship_id != UNKNOWN) {
    auto &relship = gdb_->rship_by_id(relship_id);
    if (relship.rship_label == lcode)
      consumer(relship);
    relship_id = relship.next_src_rship;
  }
}

void query_ctx::foreach_to_relationship_of_node(const node &n,
                                               rship_consumer_func consumer) {
  auto relship_id = n.to_rship_list;
  while (relship_id != UNKNOWN) {
    auto &relship = gdb_->rship_by_id(relship_id);
    if (relship.is_valid())
      consumer(relship);
    relship_id = relship.next_dest_rship;
  }
}

void query_ctx::foreach_to_relationship_of_node(const node &n,
                                               const std::string &label,
                                               rship_consumer_func consumer) {
  auto lc = gdb_->dict_->lookup_string(label);
  foreach_to_relationship_of_node(n, lc, consumer);
}

void query_ctx::foreach_to_relationship_of_node(const node &n, dcode_t lcode,
                                               rship_consumer_func consumer) {
  auto relship_id = n.to_rship_list;
  while (relship_id != UNKNOWN) {
    auto &relship = gdb_->rship_by_id(relship_id);
    if (relship.rship_label == lcode)
      consumer(relship);
    relship_id = relship.next_dest_rship;
  }
}

bool query_ctx::is_node_property(const node &n, const std::string &pkey,
                                p_item::predicate_func pred) {
  auto pc = gdb_->dict_->lookup_string(pkey);
  return is_node_property(n, pc, pred);
}

bool query_ctx::is_node_property(const node &n, dcode_t pcode,
                                p_item::predicate_func pred) {
  auto val = gdb_->node_properties_->property_value(n.property_list, pcode);
  return val.empty() ? false : pred(val);
}

bool query_ctx::is_relationship_property(const relationship &r,
                                        const std::string &pkey,
                                        p_item::predicate_func pred) {
  auto pc = gdb_->dict_->lookup_string(pkey);
  return is_relationship_property(r, pc, pred);
}

bool query_ctx::is_relationship_property(const relationship &r, dcode_t pcode,
                                        p_item::predicate_func pred) {
  auto val = gdb_->rship_properties_->property_value(r.id(), pcode);
  return val.empty() ? false : pred(val);
}