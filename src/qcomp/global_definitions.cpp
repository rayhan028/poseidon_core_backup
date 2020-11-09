#include "global_definitions.hpp"

//void(*)(graph_db*, int, int**, int*, int, int*, int**, int);
// {gdb, next_oid, forward, rs, qr_size, ty, call_map_arg, offset});
/*extern "C" void get_nodes(graph_db *gdb, char *label, consumer_fct_type consumer) {
    qr_arr qrl;
    gdb->nodes_by_label(std::string(label), [&](node &n) {
        consumer(gdb, {&n});
    });
}*/


void call_consumer_function(consumer_fct_type consumer, graph_db* gdb, int tid, int** qr, int* rs, int size, int* tyvec, int** consumer_) {
    std::cout << "call consumer" << std::endl;
    consumer(gdb, tid, qr, rs, size, tyvec, consumer_, 0);
}

extern "C" void printNode(node *n) {
    std::cout << "from printNode: " << n->node_label << std::endl;
}

extern "C" void print_int(int i) {
    std::cout << "Int is " << i << std::endl;
}

static int CONSUME_COUNTER = 0;

extern "C" void stupid() {
    //std::cout << "CALLED: " << CONSUME_COUNTER++ << std::endl;
}


extern "C" void consumerDummy(qres *qr) {
    CONSUME_COUNTER++;
    //std::cout << "Consumer called " << CONSUME_COUNTER << " times. " << std::endl;
    //std::cout << "Consumed type: " << qr->type << " and null: " << qr->is_null << " with label: " << reinterpret_cast<node*>(qr->res)->node_label << std::endl;
}

extern "C" void list_size(qr_list *list) {
    //std::cout << "Size of list is: " << list->size << std::endl;
    auto head = list->head;
    int i = 0;
    while (head != nullptr) {
        i++;
        //std::cout << i << " " << head << " is of type: " << head->res->type << std::endl;
        head = head->next;
    }
    //std::cout << "Size of iterate list is: " << i << std::endl;
}


#include <boost/thread/barrier.hpp>
boost::barrier bar(std::thread::hardware_concurrency());

extern "C" chunked_vec<node, NODE_CHUNK_SIZE>::range_iter *get_vec_begin(node_list *vec, size_t first, size_t last) {
    //bar.wait();

    return new chunked_vec<node, NODE_CHUNK_SIZE>::range_iter(vec->as_vec(), first, last);
}

extern "C" chunked_vec<node, NODE_CHUNK_SIZE>::range_iter *get_vec_next(chunked_vec<node, NODE_CHUNK_SIZE>::range_iter *it) {
    //std::cout << "Get next" << std::endl;
    return &it->operator++();;
}

extern "C" bool vec_end_reached(node_list &vec, chunked_vec<node, NODE_CHUNK_SIZE>::range_iter *it) {
    //std::cout << "test end" << std::endl;
    return !it->operator bool();
}

extern "C" chunked_vec<relationship, RSHIP_CHUNK_SIZE>::iter get_vec_begin_r(relationship_list &vec) {
    return vec.as_vec().begin();
}

extern "C" chunked_vec<relationship, RSHIP_CHUNK_SIZE>::iter *get_vec_next_r(chunked_vec<relationship, RSHIP_CHUNK_SIZE>::iter *it) {
    //std::cout << "Next called" << std::endl;
    //std::cout << "it get" << std::endl;
    return &it->operator++();
}

extern "C" bool vec_end_reached_r(relationship_list &vec, chunked_vec<relationship, RSHIP_CHUNK_SIZE>::iter it) {
    //std::cout << "vec_end_reached: " << std::endl;
    return !(it != vec.as_vec().end());
}

extern "C" dcode_t dict_lookup_label(graph_db *gdb, char *label) {
    return gdb->get_code(label);
}

extern "C" node *get_node_from_it(chunked_vec<node, NODE_CHUNK_SIZE>::range_iter *it) {
    return &it->operator*();
}

extern "C" relationship *get_rship_from_it(chunked_vec<relationship, RSHIP_CHUNK_SIZE>::iter *it) {
    //std::cout << "get rship" << std::endl;
    return &it->operator*();
}

extern "C" chunked_vec<node, NODE_CHUNK_SIZE> *gdb_get_nodes(graph_db *gdb) {
    return &gdb->get_nodes()->as_vec();
}

extern "C" chunked_vec<relationship, RSHIP_CHUNK_SIZE> *gdb_get_rships(graph_db *gdb) {
    return &gdb->get_relationships()->as_vec();
}

extern "C" void test_ints(uint64_t a, uint64_t b) {
    std::cout << "A: " << a << " B: " << b << "EQ: " << (a == b) << std::endl;
}

extern "C" relationship *rship_by_id(graph_db *gdb, offset_t id) {
    return &gdb->rship_by_id(id);
}

extern "C" node *node_by_id(graph_db *gdb, offset_t id) {
    return &gdb->node_by_id(id);
}

extern "C" dcode_t gdb_get_dcode(graph_db *gdb, char *property) {
    //std::cout << "Search for property: " << property  << " with code: " << gdb->get_code(property) << std::endl;
    return gdb->get_code(property);
}

extern "C" const property_set *pset_get_item_at(graph_db *gdb, offset_t id) {
    //std::cout << "Get property at: " << id << std::endl;
    return &gdb->get_node_properties()->get(id);
}

extern "C" int get_join_vec_size(std::vector<qr_arr>* vec) {
    return vec->size();
}

int sz = 1;

extern "C" int** get_join_vec_arr(std::vector<qr_arr>* vec, int idx) {
    //return vec->data()->data();
    return vec->at(idx).data();
}

Collector clr;
graph_db *Collector::gdb = nullptr;

Joiner joiner;

thread_local std::map<int, std::string> str_result;
thread_local std::map<int, boost::posix_time::ptime> time_result;
thread_local int str_res_ctr = 0;

thread_local std::map<int, node_description> descs;
thread_local std::map<int, rship_description> rdescs;

std::map<int, std::function<std::string(graph_db*, int*)>> con_map;

extern "C" void collect(graph_db *gdb, int **qr, result_set * rs, int qr_size, std::vector<int> *types) {
    qr_tuple res(types->size());
    int j = types->size()-1;

    for(int i = types->size(); i > 0; i--) {
        auto type = types->at(i-1);
        if(type == 2) {
            //TODO: optimize
            res[j] = std::stoi(con_map[type](gdb, qr[i]));
        } else {
            res[j] = con_map[type](gdb, qr[i]);
        }

        j--;
    }

    str_res_ctr = 0;
    descs.clear();
    rs->data.push_back(res);
}


extern "C" void join_insert_left(std::vector<qr_arr>* vec, int **qrl) {
    /*qr_arr qr;
    qr.fill(&sz);
    auto size = *qrl[0];
    qr[0] = &sz;
    for(int i = 1; i <= size; i++) {
        qr[i] = qrl[i];
    }

    vec->push_back(qr);*/
}

extern "C" void check_qr(int** qr) {
    auto sz = qr[0];
    auto n = (relationship*)qr[3];
    std::cout << "Sz: " << *sz << std::endl;
    std::cout << "Label: " << n->rship_label << std::endl;
}

extern "C" qr_list *join_consume_left() {
    return joiner.consume();
}

void Collector::collect(int **rl)  {
/*    auto n = (node*)rl[1];
    auto nd = gdb->get_node_description(*n).to_string();
    auto r = (relationship*)rl[2];
    auto rd = gdb->get_rship_description(*r).to_string();
    auto n2 = (node*)rl[3];
    auto nd2 = gdb->get_node_description(*n2).to_string();
    auto r2 = (relationship*)rl[4];
    auto rd2 = gdb->get_rship_description(*r2).to_string();
    auto n3 = (node*)rl[5];
    auto nd3 = gdb->get_node_description(*n3).to_string();
    {
        std::unique_lock<std::mutex> lk {mut};
        results.push_back(nd);
        results.push_back(rd);
        results.push_back(nd2);
        called_++;
    } */
    //std::cout << "Collected list with: " << rl->size << " elements" << std::endl;
/*    auto tail = rl->tail;
    auto res = tail->res;

    qr_tuple result(rl->size);

    auto cur = rl->head;
    while(cur != nullptr) {
        if(cur->res->type == 0) {
            auto n = (node*)cur->res->res;
            auto nd = gdb->get_node_description(*n).to_string();
            result.push_back(nd);
        } else if(cur->res->type == 1) {
            auto r = (relationship*)cur->res->res;
            auto rd = gdb->get_rship_description(*r).to_string();
            result.push_back(rd);
        } else if(cur->res->type == 2) {
            result.push_back(std::to_string(*cur->res->res));
        } else if(cur->res->type == 4) {
            auto str  = reinterpret_cast<char*>(rl->tail->res->res);
            result.push_back(std::string(str));
        }
        cur = cur->next;
    }
    {
        std::unique_lock<std::mutex> lk {mut};
        results.data.push_back(result);
        called_++;
    }*/
/*
    if(res->type == 0) {
        //std::cout << "node label: " << ((node *) (rl->tail->res->res))->node_label << std::endl;
    } else if(res->type == 1) {
        //std::cout << "rship label: " << ((relationship *) (rl->tail->res->res))->rship_label << std::endl;
    } else if(res->type == 2) {
        std::cout << "int prj:  " << *rl->tail->res->res << std::endl;
    } else if(res->type == 4)
        //std::cout << "string prj:  " << (reinterpret_cast<char*>(rl->tail->res->res)) << std::endl;

*/
    //std::cout << "Called: " << called_ << " times" << std::endl;
}

void Joiner::insert(qr_list *res)  {
    left_input_.push_back(res);
    std::cout << "Size: " << left_input_.size() << std::endl;
    cur_pos_ = left_input_.begin();
}

qr_list * Joiner::consume()  {
    std::cout << "consumed " << left_input_.size() << std::endl;
    if (left_input_.size() == 0)
        return nullptr;
    auto qrl = left_input_.back();
    left_input_.pop_back();
    return qrl;
}

extern "C" xid_t get_tx(transaction_ptr tx) {
    return tx->xid();
    //current_transaction_ = std::move(tx);
    //check_tx_context();
    //return current_transaction()->xid();
}

extern "C" node * get_valid_node(graph_db *gdb, node * n, transaction_ptr tx) {
    return &gdb->get_valid_node_version(*n, tx->xid());
}

extern "C" char* get_str_property(const properties_t &p, const std::string &key) {
    auto it = p.find(key);
    if (it == p.end()) {
        spdlog::info("unknown property: {}", key);
        throw unknown_property();
    }
    auto prop_heap = new std::string;
    *prop_heap = boost::any_cast<std::string>(it->second);
    auto ret = const_cast<char*>(reinterpret_cast<const char*>(prop_heap->c_str()));
    return ret;
}

void apply_pexpr_node(graph_db *gdb, const char *key, FTYPE val_type, int *qr, int *ret) {
    auto n = (node*)qr;
    if(descs.find(n->id()) == descs.end())
        descs[n->id()] = gdb->get_node_description(n->id());
    auto nd = descs[n->id()];

    if(val_type == FTYPE::INT) {
        *ret = get_property<uint64_t>(nd.properties, key).value();
    } else if(val_type == FTYPE::STRING) {
        str_result[str_res_ctr] = boost::any_cast<std::string>(nd.properties[std::string(key)]);;
        *ret = str_res_ctr++;
    } else if(val_type == FTYPE::DATE) {
        time_result[str_res_ctr] = get_property<boost::posix_time::ptime>(nd.properties, key).value();
        *ret = str_res_ctr++;
    }

}

void apply_pexpr_rship(graph_db *gdb, const char *key, FTYPE val_type, int *qr, int *ret) {
    auto r = (relationship*)qr;
    if(rdescs.find(r->id()) == rdescs.end())
        rdescs[r->id()] = gdb->get_rship_description(r->id());
    auto rd = rdescs[r->id()];

    if(val_type == FTYPE::INT) {
        *ret = get_property<int>(rd.properties, key).value();
    }
    else if(val_type == FTYPE::STRING) {
        str_result[str_res_ctr] = boost::any_cast<std::string>(rd.properties[std::string(key)]);;
        *ret = str_res_ctr++;
    }

}
std::mutex prj_mutex;

extern "C" void apply_pexpr(graph_db *gdb, const char *key, FTYPE val_type, int *qr, int idx, std::vector<int> types, int *ret) {
    std::lock_guard<std::mutex> lock(prj_mutex);
    if(types.at(idx) == 0) { // is node
        apply_pexpr_node(gdb, key, val_type, qr, ret);

    } else if(types.at(idx) == 1) { // is rship
        apply_pexpr_rship(gdb, key, val_type, qr, ret);
    } else if(types.at(idx) == 2) { // is rship
        ret = qr;
    }
}

extern "C" const char* lookup_dc(graph_db *gdb, dcode_t dc) {
    return gdb->get_string(dc);
}

extern "C" void join_vec_insert(std::vector<int*> *inputs, int* res) {
    inputs->push_back(res);
}

extern "C" void merge_type_vec(std::vector<int*> *lhs, std::vector<int*> *rhs) {

}

extern "C" node* create_node(graph_db *gdb, char *label, properties_t *props) {
    auto node_id = gdb->add_node(std::string(label), *props, true);
    return &gdb->node_by_id(node_id);
}

extern "C" relationship* create_rship(graph_db *gdb, char *label, node *n1, node *n2, properties_t *props) {
    auto rid = gdb->add_relationship(n1->id(), n2->id(), label, *props, true);
    return &gdb->rship_by_id(rid);
}

extern "C" void foreach_variable_from(graph_db *gdb, dcode_t label, int min, int max, consumer_fct_type consumer,
                                      int oid, int **qr, int *rs, int size, int *ty, int **call_map_arg, int offset) {
    auto prev_pos = size + offset;
    auto insert_pos = prev_pos + 1;
    auto n = (node*)qr[prev_pos];
    auto nsize = size++;
    *qr[0]++;
    gdb->foreach_variable_from_relationship_of_node(*n, label, min, max, [&](relationship &r) {
        qr[insert_pos] = (int*)&r;
        consumer(gdb, oid, qr, rs, nsize, ty, call_map_arg, offset);
    });
}

extern "C" void foreach_variable_to(graph_db *gdb, dcode_t label, int min, int max, consumer_fct_type consumer,
                                      int oid, int **qr, int *rs, int size, int *ty, int **call_map_arg, int offset) {
    auto prev_pos = size + offset;
    auto insert_pos = prev_pos + 1;
    auto n = (node*)qr[prev_pos];
    auto nsize = size++;
    *qr[0]++;
    gdb->foreach_variable_to_relationship_of_node(*n, label, min, max, [&](relationship &r) {
        qr[insert_pos] = (int*)&r;
        consumer(gdb, oid, qr, rs, nsize, ty, call_map_arg, offset);
    });
}

thread_local qr_tuple tp;

std::mutex mat_reg_mut;
extern "C" void mat_reg_value(graph_db *gdb, int *reg, int type) {
    std::lock_guard<std::mutex> lck(mat_reg_mut);
    if(type == 2) {
        uint64_t x = std::stoull(con_map[type](gdb, reg));
        tp.push_back(x);
    } else if(type == 5) {
        tp.push_back(time_result[*reg]);
    } else {
        tp.push_back(con_map[type](gdb, reg));
    }

}

std::mutex ct_mut;
extern "C" void collect_tuple(result_set *rs) {
    std::lock_guard<std::mutex> lck(ct_mut);
    rs->data.push_back(tp);
    tp.clear();
}

#include "joiner.hpp"

thread_local qr_tuple mat_tuple;

extern "C" qr_tuple *obtain_mat_tuple() {
    //auto tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
    //return &joiner::mat_tuple_[tid];
    return &mat_tuple;
}

extern "C" void mat_node(node *n, qr_tuple *qr) {
    qr->push_back(n);
}

extern "C" void mat_rship(relationship *r, qr_tuple *qr) {
    qr->push_back(r);
}

extern "C" void collect_tuple_join(int jid, qr_tuple *qr) {
    joiner::materialize_rhs(jid, qr);
}

extern "C" qr_tuple *get_join_tp_at(int jid, int pos) {
    return &joiner::rhs_input_[jid].at(pos);
}

extern "C" node *get_node_res_at(qr_tuple *tuple, int pos) {
    return boost::get<node*>(tuple->at(pos));
}

extern "C" relationship *get_rship_res_at(qr_tuple *tuple, int pos) {
    return boost::get<relationship*>(tuple->at(pos));
}

extern "C" int get_mat_res_size(int jid) {
    return joiner::rhs_input_[jid].size();
}

extern "C" node *index_get_node(graph_db *gdb, char *label, char *prop, uint64_t value) {
    auto idx = gdb->get_index(std::string(label), std::string(prop));

    node *n_ptr;
    auto found = false;
    gdb->index_lookup(idx, value, [&](auto& n) {
        found = true;
        n_ptr = &n;
    });
    return n_ptr;
}
