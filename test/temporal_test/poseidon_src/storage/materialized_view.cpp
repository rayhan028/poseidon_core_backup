#include "materialized_view.hpp"
#include "relationships.hpp"
#include <unordered_set>


materialized_view::materialized_view() {
    std::cout << "Create Path Index..." << std::endl;
    path_index_ = p_make_btree();
}

void materialized_view::add_path(const qr_tuple &qr) {
    std::vector<offset_t> rship_ids;
    // create relationship id list
    for(auto & t : qr) {
        if(t.type() == typeid(relationship*)) {
            auto r = boost::get<relationship*>(t);
            rship_ids.push_back(r->id());
        }
    }

    // insert id list into storage
    offset_t prev_prim = UNKNOWN;
    auto tid = count_.fetch_add(1);
    view_primitive *last_vp = nullptr;
    for(auto id : rship_ids) {
        view_primitive vp;
        vp.type_ = 1;
        vp.primitive_id_ = id;
        vp.prev_ = prev_prim;
        vp.next_ = UNKNOWN;
        vp.tuple_id = tid;

        auto res = results_.store(std::move(vp));
        path_index_->insert(id, tid);
        auto cur_prim = res.first;

        if(prev_prim != UNKNOWN) {
            last_vp->next_ = cur_prim;
        }
        last_vp = res.second;
        prev_prim = cur_prim;
    }
    //TODO: create CSR
}

void materialized_view::scan_view(node_list &nl, relationship_list &rl, std::function<void(qr_tuple&)> consumer) {
    std::unordered_set<uint32_t> visited;

    for(auto & p : results_) {
        if(visited.contains(p.tuple_id)) {
            continue;
        }

        qr_tuple tuple;

        auto & rf = rl.get(p.primitive_id_);
        auto & n1 = nl.get(rf.from_node_id());
        auto & n2 = nl.get(rf.to_node_id());
        tuple.push_back(&n1);
        tuple.push_back(&rf);
        tuple.push_back(&n2);


        auto next = &p;
        while(next->next_ != UNKNOWN) {
            next = &results_.at(next->next_);
            auto & next_rship = rl.get(next->primitive_id_);
            auto & next_n2 = nl.get(next_rship.to_node_id());
            tuple.push_back(&next_rship);
            tuple.push_back(&next_n2);
        }

        /* tuple was completly visited, mark as already seen and skip in next iteration */
        if(next->next_ == UNKNOWN) {
            visited.insert(p.tuple_id);
        }
        consumer(tuple);
    }
}

bool materialized_view::get_path(offset_t offset, view_primitive *path) {
    offset_t path_offset;
    auto found = path_index_->lookup(offset, &path_offset);

    if(found) {
        path = &results_.at(path_offset);
        return true;
    } else {
        return false;
    }
}