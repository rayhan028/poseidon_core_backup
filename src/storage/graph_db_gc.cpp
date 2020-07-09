#include "graph_db.hpp"

void graph_db::vacuum(xid_t tx) {
    std::unique_lock<std::mutex> l(*gcm_);
    auto iter = garbage_->begin();
    bool finished = iter == garbage_->end();
    if (!finished) 
        spdlog::info("GC: starting vacuum ... {} items", garbage_->size());

    while (!finished) {
        auto& gitem = *iter;
        // remove the node/relationship physically
        if (gitem.itype == gc_item::gc_node) {
            spdlog::info("GC: delete node #{}", gitem.oid);
            auto& n = nodes_->get(gitem.oid);
            // wwe assume that we already have checked that n can be removed
            // assert(n.from_rship_list == UNKNOWN && n.to_rship_list == UNKNOWN);
            // furthermore, the dirty list should be empty
            assert(!n.has_dirty_versions());
            nodes_->remove(gitem.oid);
        }
        else {
            spdlog::info("GC: delete rship #{}", gitem.oid);
            // remove rel_id from the list of relationships
            auto& rship = rships_->get(gitem.oid);
            assert(!rship.has_dirty_versions());
            auto& n1 = nodes_->get(rship.src_node);

            if (n1.from_rship_list == gitem.oid) {
                n1.from_rship_list = rship.next_src_rship;
                spdlog::info("F      update node {} --> rel = {} -> {}", 
                    rship.src_node, n1.from_rship_list, rship.next_src_rship);
            }
            else {
                // set the field next_src_rship of the previous relationship to our next_src_rship
                auto prev_id = n1.from_rship_list;
                while (prev_id != UNKNOWN) {
                    auto &p_relship = rships_->get(prev_id);
                    if (p_relship.next_src_rship == gitem.oid) {
                        spdlog::info("F      update previous relationship {} of node {} to {}", 
                            prev_id, rship.src_node, rship.next_src_rship);
                        p_relship.next_src_rship = rship.next_src_rship;
                        break;    
                    }
                    prev_id = p_relship.next_src_rship;
                }
            }
            auto& n2 = nodes_->get(rship.dest_node);
            if (n2.to_rship_list == gitem.oid) {
                spdlog::info("T      update node {} --> rel = {} -> {}", rship.dest_node, 
                     n2.to_rship_list, rship.next_dest_rship);
                n2.to_rship_list = rship.next_dest_rship;
            }
            else {
                // set the field next_dest_rship of the previous relationship to our next_dest_rship
                auto prev_id = n2.to_rship_list;
                while (prev_id != UNKNOWN) {
                    auto &p_relship = rships_->get(prev_id);
                    if (p_relship.next_dest_rship == gitem.oid) {
                        spdlog::info("T      update previous relationship {} of node {} to {}", 
                            prev_id, rship.dest_node, rship.next_dest_rship);
                        p_relship.next_dest_rship = rship.next_dest_rship;
                        break;    
                    }
                    prev_id = p_relship.next_dest_rship;
                }
            }
            rships_->remove(gitem.oid);
        }
        iter++;
        finished = iter == garbage_->end();
    }
    garbage_->erase(garbage_->begin(), iter);
}