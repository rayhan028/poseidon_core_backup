#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <functional>
#include <chrono>
#include <filesystem>
#include <random> 
#include <cmath>
#include <algorithm>
#include <thread>
#include <spdlog/spdlog.h>

#include "graph_db.hpp"
#include "query_ctx.hpp"
#include "qop_as_of_scan.hpp"
#include "qop_temporal_expand.hpp"
#include "qop_temporal_project.hpp"
#include "qop_temporal_filter.hpp"
#include "loader_ldbc.hpp" 

using namespace std::placeholders;

// --- CONFIG ---
const std::vector<int> OPS_STAGES = {200000, 400000, 600000, 800000, 1000000};
const int BENCHMARK_ITERATIONS_PER_STAGE = 20;
const int SAMPLES_PER_ITER = 100;
const uint64_t LDBC_BASE_TIME = 1800000000000ULL;

const std::string QUERY_CSV = "/home/rayhan/poseidon_core/test/temporal_test/benchmark_data_T_LDBC_sf1/poseidon_query_latencies.csv";
const std::string STORAGE_CSV = "/home/rayhan/poseidon_core/test/temporal_test/benchmark_data_T_LDBC_sf1/poseidon_storage_and_write.csv";

struct RandomHelper {
    static std::string ranstr(int len) {
        static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        std::string s = "";
        for (int i = 0; i < len; ++i) s += alphanum[rand() % (sizeof(alphanum) - 1)];
        return s;
    }
    static std::string generate_ip() {
        return std::to_string(rand()%255)+"."+std::to_string(rand()%255)+"."+std::to_string(rand()%255)+"."+std::to_string(rand()%255);
    }
};

long get_current_rss_mb() {
    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line)) {
        if (line.rfind("VmRSS:", 0) == 0) {
            std::istringstream iss(line);
            std::string key, unit;
            long value_kb;
            iss >> key >> value_kb >> unit;
            return value_kb / 1024; 
        }
    }
    return -1;
}

class ZipfGenerator { 
public:
    ZipfGenerator(size_t n, double alpha) {
        if (n == 0) n = 1;
        std::vector<double> weights;
        for (size_t i = 1; i <= n; ++i) weights.push_back(1.0 / std::pow((double)i, alpha)); 
        dist = std::discrete_distribution<size_t>(weights.begin(), weights.end()); 
    }
    size_t next(std::default_random_engine& gen) { return dist(gen); } 
private:
    std::discrete_distribution<size_t> dist; 
};

struct TupleSource : public qop { 
    std::vector<uint64_t> ids_;  
    TupleSource(const std::unordered_set<uint64_t>& ids) : ids_(ids.begin(), ids.end()) { type_ = qop_type::scan; }
    void start(query_ctx &ctx) override {
        for (auto id : ids_) {
            qr_tuple t; t.push_back(id);
            if (consume_) consume(ctx, t);
        }
        qop::default_finish(ctx);
    }
    void accept(qop_visitor &vis) override { if (has_subscriber()) subscriber_->accept(vis); }
    void dump(std::ostream &os) const override { os << "TupleSource"; }
};

struct SilentPrinter : public qop { 
    void process(query_ctx &ctx, const qr_tuple &v) { }
    void finish(query_ctx &ctx) { }
    void accept(qop_visitor &vis) override { if (has_subscriber()) subscriber_->accept(vis); }
    void dump(std::ostream &os) const override { os << "SilentPrinter"; }
};

void run_bench_stage(std::shared_ptr<graph_db> db, const std::vector<uint64_t>& p_lids, const std::vector<uint64_t>& m_lids,
                    uint64_t current_ops_count, int iteration) {
    
    std::default_random_engine local_gen(42 + iteration);
    ZipfGenerator p_zipf(p_lids.size(), 1.1);
    ZipfGenerator m_zipf(m_lids.size(), 1.1);
    auto sink = std::make_shared<SilentPrinter>();

    auto measure_sample = [&](const std::vector<uint64_t>& source_ids, ZipfGenerator& zipf, auto execute_func) {
        auto t0 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < SAMPLES_PER_ITER; ++i) {
            uint64_t id = source_ids[zipf.next(local_gen) % source_ids.size()];           
            uint64_t random_ts = LDBC_BASE_TIME + (rand() % (current_ops_count + 1));
            try {
                db->begin_transaction();
                execute_func(id, random_ts); 
                db->commit_transaction();
            } catch (...) { try { db->commit_transaction(); } catch(...) {} }
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count() / (1000.0 * SAMPLES_PER_ITER);
    };

    auto is1 = [&](uint64_t target_id, uint64_t query_vt) {
        query_ctx ctx(db);
        auto source = std::make_shared<TupleSource>(std::unordered_set<uint64_t>{target_id});
        auto scan = std::make_shared<poseidon::qop_as_of_scan>(query_vt, true);
        auto expand = std::make_shared<poseidon::qop_temporal_expand>(query_vt, true, "IS_LOCATED_IN");
        auto proj = std::make_shared<poseidon::qop_temporal_project>(std::vector<std::string>{"locationIP", "browserUsed"}, 0);
        source->connect(scan, std::bind(&poseidon::qop_as_of_scan::process, scan.get(), _1, _2), nullptr);
        scan->connect(expand, std::bind(&poseidon::qop_temporal_expand::process, expand.get(), _1, _2), nullptr);
        expand->connect(proj, std::bind(&poseidon::qop_temporal_project::process, proj.get(), _1, _2), nullptr);
        proj->connect(sink, std::bind(&SilentPrinter::process, sink.get(), _1, _2), nullptr);
        source->start(ctx);
    };

    auto is3 = [&](uint64_t target_id, uint64_t query_vt) {
        query_ctx ctx(db);
        auto source = std::make_shared<TupleSource>(std::unordered_set<uint64_t>{target_id});
        auto scan = std::make_shared<poseidon::qop_as_of_scan>(query_vt, true);
        auto expand = std::make_shared<poseidon::qop_temporal_expand>(query_vt, true, "KNOWS");
        auto proj = std::make_shared<poseidon::qop_temporal_project>(std::vector<std::string>{"firstName", "lastName"}, 0);
        source->connect(scan, std::bind(&poseidon::qop_as_of_scan::process, scan.get(), _1, _2), nullptr);
        scan->connect(expand, std::bind(&poseidon::qop_temporal_expand::process, expand.get(), _1, _2), nullptr);
        expand->connect(proj, std::bind(&poseidon::qop_temporal_project::process, proj.get(), _1, _2), nullptr);
        proj->connect(sink, std::bind(&SilentPrinter::process, sink.get(), _1, _2), nullptr);
        source->start(ctx);
    };

    auto is4 = [&](uint64_t target_id, uint64_t query_vt) {
        query_ctx ctx(db);
        auto source = std::make_shared<TupleSource>(std::unordered_set<uint64_t>{target_id});
        auto scan = std::make_shared<poseidon::qop_as_of_scan>(query_vt, true);
        auto proj = std::make_shared<poseidon::qop_temporal_project>(std::vector<std::string>{"content"}, 0);
        source->connect(scan, std::bind(&poseidon::qop_as_of_scan::process, scan.get(), _1, _2), nullptr);
        scan->connect(proj, std::bind(&poseidon::qop_temporal_project::process, proj.get(), _1, _2), nullptr);
        proj->connect(sink, std::bind(&SilentPrinter::process, sink.get(), _1, _2), nullptr);
        source->start(ctx);
    };

    auto is5 = [&](uint64_t target_id, uint64_t query_vt) {
        query_ctx ctx(db);
        auto source = std::make_shared<TupleSource>(std::unordered_set<uint64_t>{target_id});
        auto scan = std::make_shared<poseidon::qop_as_of_scan>(query_vt, true);
        auto expand = std::make_shared<poseidon::qop_temporal_expand>(query_vt, true, "HAS_CREATOR");
        auto proj = std::make_shared<poseidon::qop_temporal_project>(std::vector<std::string>{"firstName", "lastName"}, 0);
        source->connect(scan, std::bind(&poseidon::qop_as_of_scan::process, scan.get(), _1, _2), nullptr);
        scan->connect(expand, std::bind(&poseidon::qop_temporal_expand::process, expand.get(), _1, _2), nullptr);
        expand->connect(proj, std::bind(&poseidon::qop_temporal_project::process, proj.get(), _1, _2), nullptr);
        proj->connect(sink, std::bind(&SilentPrinter::process, sink.get(), _1, _2), nullptr);
        source->start(ctx);
    };

    double avg_is1 = measure_sample(p_lids, p_zipf, is1);
    double avg_is3 = measure_sample(p_lids, p_zipf, is3);
    double avg_is4 = measure_sample(m_lids, m_zipf, is4);
    double avg_is5 = measure_sample(m_lids, m_zipf, is5);

    std::ofstream q_out(QUERY_CSV, std::ios_base::app);
    q_out << iteration << "," << current_ops_count << "," << avg_is1 << "," << avg_is3 << "," << avg_is4 << "," << avg_is5 << "\n";
    std::cout << "Iter: " << iteration << " | IS1: " << avg_is1 << " | IS4: " << avg_is4 << " ms" << std::endl;
}

int main(int argc, char* argv[]) {
    spdlog::set_level(spdlog::level::err);
    std::string sf = (argc > 1) ? argv[1] : "sf1";
    auto db_ptr = std::make_shared<graph_db>("poseidon_ldbc_" + sf, ".", 0);
    std::string base = "/home/rayhan/poseidon_core/Temporal-data-LDBC/" + sf + "_enhanced/";
    const uint64_t FUTURE_BASE_TIME = LDBC_BASE_TIME;

    // Header Initialization - RSS removed as requested
    std::ofstream s_file(STORAGE_CSV); 
    s_file << "Total_Ops,Avg_Write_Latency_ms,Current_Data_MB,Historical_Data_MB\n";
    s_file.close();
    std::ofstream q_file(QUERY_CSV); 
    q_file << "Iteration,Ops_Count,Avg_IS1_ms,Avg_IS3_ms,Avg_IS4_ms,Avg_IS5_ms\n"; 
    q_file.close();

    std::cout << ">>> STAGE 1: Loading Initial Dataset (SF1)..." << std::endl;
    // Load Nodes
    load_ldbc_node(*db_ptr, base + "dynamic/person_0_0.csv", "Person");
    load_ldbc_node(*db_ptr, base + "dynamic/comment_0_0.csv", "Comment");
    load_ldbc_node(*db_ptr, base + "dynamic/post_0_0.csv", "Post");
    load_ldbc_node(*db_ptr, base + "dynamic/forum_0_0.csv", "Forum");
    load_ldbc_node(*db_ptr, base + "static/organisation_0_0.csv", "Organisation");
    load_ldbc_node(*db_ptr, base + "static/place_0_0.csv", "Place");
    load_ldbc_node(*db_ptr, base + "static/tag_0_0.csv", "Tag");
    load_ldbc_node(*db_ptr, base + "static/tagclass_0_0.csv", "TagClass");

    // Load Edges
    load_ldbc_edge(*db_ptr, base + "dynamic/person_knows_person_0_0.csv", "KNOWS", "Person", "Person");
    load_ldbc_edge(*db_ptr, base + "dynamic/comment_hasCreator_person_0_0.csv", "HAS_CREATOR", "Comment", "Person");
    load_ldbc_edge(*db_ptr, base + "dynamic/post_hasCreator_person_0_0.csv", "HAS_CREATOR", "Post", "Person");
    load_ldbc_edge(*db_ptr, base + "dynamic/forum_hasMember_person_0_0.csv", "HAS_MEMBER", "Forum", "Person");
    load_ldbc_edge(*db_ptr, base + "dynamic/comment_replyOf_post_0_0.csv", "REPLY_OF", "Comment", "Post");
    load_ldbc_edge(*db_ptr, base + "dynamic/comment_replyOf_comment_0_0.csv", "REPLY_OF", "Comment", "Comment");
    load_ldbc_edge(*db_ptr, base + "dynamic/person_likes_post_0_0.csv", "LIKES", "Person", "Post");
    load_ldbc_edge(*db_ptr, base + "dynamic/person_likes_comment_0_0.csv", "LIKES", "Person", "Comment");
    load_ldbc_edge(*db_ptr, base + "dynamic/comment_hasTag_tag_0_0.csv", "HAS_TAG", "Comment", "Tag");
    load_ldbc_edge(*db_ptr, base + "dynamic/person_isLocatedIn_place_0_0.csv", "IS_LOCATED_IN", "Person", "Place");
    load_ldbc_edge(*db_ptr, base + "dynamic/post_isLocatedIn_place_0_0.csv", "IS_LOCATED_IN", "Post", "Place");
    load_ldbc_edge(*db_ptr, base + "dynamic/forum_containerOf_post_0_0.csv", "CONTAINER_OF", "Forum", "Post");
    load_ldbc_edge(*db_ptr, base + "dynamic/post_hasTag_tag_0_0.csv", "HAS_TAG", "Post", "Tag");
    load_ldbc_edge(*db_ptr, base + "dynamic/forum_hasTag_tag_0_0.csv", "HAS_TAG", "Forum", "Tag");
    load_ldbc_edge(*db_ptr, base + "dynamic/person_hasInterest_tag_0_0.csv", "HAS_INTEREST", "Person", "Tag");
    load_ldbc_edge(*db_ptr, base + "dynamic/forum_hasModerator_person_0_0.csv", "HAS_MODERATOR", "Forum", "Person");
    load_ldbc_edge(*db_ptr, base + "static/organisation_isLocatedIn_place_0_0.csv", "IS_LOCATED_IN", "Organisation", "Place");
    load_ldbc_edge(*db_ptr, base + "static/place_isPartOf_place_0_0.csv", "IS_PART_OF", "Place", "Place");
    load_ldbc_edge(*db_ptr, base + "static/tag_hasType_tagclass_0_0.csv", "HAS_TYPE", "Tag", "TagClass");
    load_ldbc_edge(*db_ptr, base + "static/tagclass_isSubclassOf_tagclass_0_0.csv", "IS_SUBCLASS_OF", "TagClass", "TagClass");
    load_ldbc_edge(*db_ptr, base + "dynamic/person_workAt_organisation_0_0.csv", "WORK_AT", "Person", "Organisation");
    load_ldbc_edge(*db_ptr, base + "dynamic/person_studyAt_organisation_0_0.csv", "STUDY_AT", "Person", "Organisation");

    /* auto hist_mgr = db_ptr->get_history_manager();
    auto storage = hist_mgr->get_storage();

    auto get_hist_logical_size = [&]() {
        size_t total_bytes = 0;
        // Structural Metadata (Records for every version)
        total_bytes += storage->node_deltas_.size() * sizeof(node_history_record);
        total_bytes += storage->rship_deltas_.size() * sizeof(rship_history_record);

        // Index Chain overhead (Mapping logic to history)
        for (auto const& [lid, chain] : storage->node_delta_index_) 
            total_bytes += chain.size() * sizeof(offset_t);
        for (auto const& [lid, chain] : storage->rship_delta_index_) 
            total_bytes += chain.size() * sizeof(offset_t);

        // Historical Delta Properties (Unique to History)
        auto count_unique_deltas = [&](const auto& delta_vec) {
            size_t d_bytes = 0;
            for (const auto& record : delta_vec) {
                // Only count memory that was created as a result of an UPDATE
                if (record.delta_pid != 0 && record.delta_pid != static_cast<uint32_t>(-1)) {
                    d_bytes += sizeof(property_set);
                }
            }
            return d_bytes;
        };

        total_bytes += count_unique_deltas(storage->node_deltas_);
        total_bytes += count_unique_deltas(storage->rship_deltas_);

        return total_bytes;
    };

    auto get_curr_logical_size = [&]() {
    // Current data is logically the set of Anchors (one per active node/edge)
        size_t n_count = active_nodes.size();
        size_t r_count = db_ptr->get_relationships()->as_vec().size(); // Approximate for SF1
        
        // Properties physically belonging to the "Current" graph state
        size_t n_bytes  = n_count * sizeof(node);
        size_t r_bytes  = r_count * sizeof(relationship);
        size_t p_bytes  = (n_count + r_count) * sizeof(property_set);       
        return n_bytes + r_bytes + p_bytes;
    };

    // Populate active lists for random operations
    std::vector<uint64_t> p_lids, m_lids, active_nodes;
    db_ptr->begin_transaction();
    for(auto lid : db_ptr->get_active_node_lids()) {
        auto phys = db_ptr->find_current_node(lid);
        if (phys != UNKNOWN) {
            auto desc = db_ptr->get_node_description(phys);
            if (desc.label == "Person") p_lids.push_back(lid);
            else if (desc.label == "Post" || desc.label == "Comment") m_lids.push_back(lid);
            active_nodes.push_back(lid);
        }
    }
    db_ptr->commit_transaction();*/

    auto hist_mgr = db_ptr->get_history_manager();
    auto storage = hist_mgr->get_storage();

    // --- FIX 1: DECLARE VECTORS HERE (Before the lambdas) ---
    std::vector<uint64_t> p_lids, m_lids, active_nodes;

    // --- FIX 2: HELPER TO COUNT USED SLOTS ---
    auto count_used_slots = [](const auto& list_ptr) {
        size_t used = 0;
        auto& v = list_ptr->as_vec(); 
        for (uint64_t i = 0; i < v.capacity(); i++) {
            if (v.is_used(i)) used++;
        }
        return used;
    };

   auto get_hist_logical_size = [&]() {
    size_t total_bytes = 0;
    auto count_actual_history = [&](const auto& delta_vec) {
        size_t bytes = 0;
        for (const auto& record : delta_vec) {
            // FIX: Only count versions created by the UPDATE workload (vt > base)
            if (record.delta_pid != 0 && record.delta_pid != static_cast<uint32_t>(-1)) {
                bytes += sizeof(record); // Metadata record
                if (record.delta_pid != 0 && record.delta_pid != static_cast<uint32_t>(-1)) {
                    bytes += sizeof(property_set); // Actual Property Delta
                }
            }
        }
        return bytes;
    };
    total_bytes += count_actual_history(storage->node_deltas_);
    total_bytes += count_actual_history(storage->rship_deltas_);
    return total_bytes;
    };

    auto get_curr_logical_size = [&]() {
        // Use the manual count helper for relationships since .size() doesn't exist
        size_t n_count = active_nodes.size();
        size_t r_count = count_used_slots(db_ptr->get_relationships());
        
        size_t n_bytes  = n_count * sizeof(node);
        size_t r_bytes  = r_count * sizeof(relationship);
        size_t p_bytes  = (n_count + r_count) * sizeof(property_set); 
        
        return n_bytes + r_bytes + p_bytes;
    };

    // --- POPULATE ACTIVE LISTS (Existing logic) ---
    db_ptr->begin_transaction();
    for(auto lid : db_ptr->get_active_node_lids()) {
        auto phys = db_ptr->find_current_node(lid);
        if (phys != UNKNOWN) {
            auto desc = db_ptr->get_node_description(phys);
            if (desc.label == "Person") p_lids.push_back(lid);
            else if (desc.label == "Post" || desc.label == "Comment") m_lids.push_back(lid);
            active_nodes.push_back(lid);
        }
    }
    db_ptr->commit_transaction();

    std::default_random_engine gen(42);
    std::uniform_int_distribution<int> mix_dist(1, 100);
    int last_ops = 0;

    for (int target_ops : OPS_STAGES) {
        int ops_to_add = target_ops - last_ops;
        ZipfGenerator node_zipf(active_nodes.size(), 1.1); 

        std::cout << "\n>>> STAGE 2: Appending " << ops_to_add << " operations..." << std::endl;
        double total_write_ns = 0; 
        int successful_writes = 0;

        for (int i = 0; i < ops_to_add; ++i) {
            //uint64_t vt = FUTURE_BASE_TIME + last_ops + i + 1; 
            uint64_t vt = 2000000000000ULL + last_ops + i;
            int roll = mix_dist(gen);
            try {
                db_ptr->begin_transaction(); 
                bool did_work = false;
                if (roll <= 80 && !active_nodes.empty()) { // UPDATE
                    uint64_t lid = active_nodes[node_zipf.next(gen) % active_nodes.size()];
                    auto phys = db_ptr->find_current_node(lid);
                    if (phys != UNKNOWN) {
                        auto desc = db_ptr->get_node_description(phys);
                        properties_t props;
                        if (desc.label == "Person") {
                            props["locationIP"] = RandomHelper::generate_ip();
                            props["browserUsed"] = (rand()%2==0)?"Firefox":"Chrome";
                        } else {
                            props["content"] = RandomHelper::ranstr(5); 
                        }
                        auto t0 = std::chrono::high_resolution_clock::now();
                        db_ptr->update_node_create_version(phys, props, vt);
                        total_write_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()-t0).count();
                        did_work = true;
                    }
                } 
                else if (roll <= 90) { // ADD
                    if (rand() % 2 == 0 || active_nodes.empty()) {
                        uint64_t new_lid = 50000000 + last_ops + i;
                        properties_t props = {{"firstName", RandomHelper::ranstr(6)}, {"locationIP", RandomHelper::generate_ip()}};
                        auto t0 = std::chrono::high_resolution_clock::now();
                        db_ptr->add_node("Person", props, vt, 253402300799000ULL, new_lid);
                        total_write_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()-t0).count();
                        did_work = true;
                        active_nodes.push_back(new_lid);
                        p_lids.push_back(new_lid); 
                    } else if (!p_lids.empty()) {
                        uint64_t src = p_lids[rand() % p_lids.size()];
                        uint64_t dst = p_lids[rand() % p_lids.size()];
                        auto phys_src = db_ptr->find_current_node(src);
                        auto phys_dst = db_ptr->find_current_node(dst);
                        if (phys_src != UNKNOWN && phys_dst != UNKNOWN) {
                            auto t0 = std::chrono::high_resolution_clock::now();
                            db_ptr->add_relationship(phys_src, phys_dst, "KNOWS", {}, vt, 253402300799000ULL, 80000000 + last_ops + i);
                            total_write_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()-t0).count();
                            did_work = true;
                        }
                    }
                } 
                else { // DELETE
                    if (!active_nodes.empty()) {
                        uint64_t lid = active_nodes[rand() % active_nodes.size()];
                        if (db_ptr->find_current_node(lid) != UNKNOWN) {
                            auto t0 = std::chrono::high_resolution_clock::now();
                            db_ptr->temporal_detach_delete(lid, vt); 
                            total_write_ns += std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now()-t0).count();
                            did_work = true;                           
                            active_nodes.erase(std::remove(active_nodes.begin(), active_nodes.end(), lid), active_nodes.end());
                            p_lids.erase(std::remove(p_lids.begin(), p_lids.end(), lid), p_lids.end());
                            m_lids.erase(std::remove(m_lids.begin(), m_lids.end(), lid), m_lids.end());
                        }
                    }
                }
                if (did_work) successful_writes++;
                db_ptr->commit_transaction(); 
            } catch(...) {}
        }

       // Manual pause to allow background threads to catch up
        std::cout << ">>> Waiting 10s for background archival..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));

        // MEASURE LOGICAL TOTALS (No subtraction, Double precision)
        double current_mb = (double)get_curr_logical_size() / (1024.0 * 1024.0);
        double historical_mb = (double)get_hist_logical_size() / (1024.0 * 1024.0);
        double avg_write_latency_ms = (successful_writes > 0) ? (total_write_ns / 1000000.0) / successful_writes : 0;

        // Logging
        std::ofstream ws_out(STORAGE_CSV, std::ios_base::app);
        ws_out << target_ops << "," << std::fixed << std::setprecision(4) << avg_write_latency_ms << "," << std::setprecision(3) << current_mb << "," << historical_mb << "\n";
        ws_out.close();

        std::this_thread::sleep_for(std::chrono::seconds(60)); 
        for (int iter = 1; iter <= BENCHMARK_ITERATIONS_PER_STAGE; ++iter) {
             run_bench_stage(db_ptr, p_lids, m_lids, target_ops, iter);
        }
        last_ops = target_ops;
    }
    return 0;
}