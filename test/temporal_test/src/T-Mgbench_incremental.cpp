#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include <memory>
#include <functional>
#include <chrono>
#include <filesystem>
#include <sys/resource.h>
#include <random> 
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <thread>
#include <map>  
#include <spdlog/spdlog.h>
#include "graph_db.hpp"
#include "query_ctx.hpp"
#include "qop_as_of_scan.hpp"
#include "qop_timeline_scan.hpp"
#include "qop_temporal_expand.hpp"
#include "qop_time_slice_expand.hpp"
#include "loader.hpp" 
#include "history_storage.hpp"

namespace fs = std::filesystem;

// CONFIGURATION
const std::vector<int> OPS_STAGES = {80000, 160000, 240000, 320000, 400000};
const int BENCHMARK_ITERATIONS_PER_STAGE = 5;
const int WARMUP = 10;
const int SAMPLES_PER_ITER = 100; 
const std::string CSV_FILE = "/home/rayhan/poseidon_core/test/temporal_test/benchmark_data_TMgbench/poseidon_final_results_T_mgBench.csv";
const std::string WRITE_LATENCY_CSV = "/home/rayhan/poseidon_core/test/temporal_test/benchmark_data_TMgbench/poseidon_write_latency_results.csv";
const std::string FIG_C_CSV = "/home/rayhan/poseidon_core/test/temporal_test/benchmark_data_TMgbench/figure_c_vertex_types.csv";
const std::string FIG_D_CSV = "/home/rayhan/poseidon_core/test/temporal_test/benchmark_data_TMgbench/figure_d_time_slice.csv";

// GLOBAL TRACKER FOR UPDATE FREQUENCY
std::unordered_map<uint64_t, uint64_t> version_tracker; 

// LOGICAL MEMORY MEASUREMENT
double get_total_logical_mb(std::shared_ptr<graph_db> db) {
    size_t total_bytes = 0;
    auto hist_mgr = db->get_history_manager();
    auto storage = hist_mgr->get_storage();

    auto count_active = [](auto list_ptr) {
        size_t count = 0;
        auto& vec = list_ptr->as_vec();
        for (uint64_t i = 0; i < vec.capacity(); ++i) {
            if (vec.is_used(i)) count++;
        }
        return count;
    };

    total_bytes += count_active(db->get_nodes()) * sizeof(node);
    total_bytes += count_active(db->get_relationships()) * sizeof(relationship);
    total_bytes += count_active(db->get_node_props()) * sizeof(property_set);
    total_bytes += count_active(db->get_rship_props()) * sizeof(property_set);

    total_bytes += storage->node_deltas_.size() * sizeof(node_history_record);
    total_bytes += storage->rship_deltas_.size() * sizeof(rship_history_record);

    auto count_hist_props = [](const auto& delta_vec) {
        size_t p_bytes = 0;
        for (const auto& record : delta_vec) {
            if (record.base_property_list != static_cast<uint64_t>(-1)) {
                p_bytes += sizeof(property_set);
            }
            if (record.delta_pid != 0 && record.delta_pid != static_cast<uint32_t>(-1)) {
                p_bytes += sizeof(property_set);
            }
        }
        return p_bytes;
    };
    total_bytes += count_hist_props(storage->node_deltas_);
    total_bytes += count_hist_props(storage->rship_deltas_);

    for (auto const& [lid, chain] : storage->node_delta_index_) {
        total_bytes += chain.size() * sizeof(uint64_t);
    }
    for (auto const& [lid, chain] : storage->rship_delta_index_) {
        total_bytes += chain.size() * sizeof(uint64_t);
    }

    return static_cast<double>(total_bytes) / (1024.0 * 1024.0);
}

// OPERATORS
struct SilentPrinter : public qop { 
    void process(query_ctx &ctx, const qr_tuple &v) { }
    void finish(query_ctx &ctx) {}
    void accept(qop_visitor &vis) override { if (has_subscriber()) subscriber_->accept(vis); }
    void dump(std::ostream &os) const override { os << "SilentPrinter"; }
};

class ZipfGenerator { 
public:
    ZipfGenerator(size_t n, double alpha) {
        std::vector<double> weights;
        for (size_t i = 1; i <= n; ++i) 
            weights.push_back(1.0 / std::pow(static_cast<double>(i), alpha)); 
        dist = std::discrete_distribution<size_t>(weights.begin(), weights.end()); 
    }
    size_t next(std::default_random_engine& gen) { return dist(gen); } 
private:
    std::discrete_distribution<size_t> dist; 
};

void run_bench_stage(std::shared_ptr<graph_db> db, const std::vector<uint64_t>& active_nodes, uint64_t vt, int iteration, int current_ops) {
    if (active_nodes.empty()) return;

    std::default_random_engine local_gen(42 + iteration);
    ZipfGenerator query_zipf(active_nodes.size(), 1.1);
    auto sink = std::make_shared<SilentPrinter>();

    std::vector<uint64_t> d_windows = {1000, 50000, 100000, 150000, 200000}; 
    constexpr uint64_t FIXED_WINDOW = 50000; 
    uint64_t start_window = (vt > FIXED_WINDOW) ? (vt - FIXED_WINDOW) : 0;

    
    // Step 1: Collect vertices that have been updated at least once
    std::vector<std::pair<uint64_t, uint64_t>> updated_vertices; // (lid, update_count)
    for (const auto& [lid, count] : version_tracker) {
        if (count > 0) {
            updated_vertices.push_back({lid, count});
        }
    }
    
    // Step 2: Sort by update count (descending)
    std::sort(updated_vertices.begin(), updated_vertices.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Step 3: Calculate total accesses
    uint64_t total_accesses = 0;
    for (const auto& [lid, count] : updated_vertices) {
        total_accesses += count;
    }
    
    if (total_accesses == 0) {
        std::cout << "WARNING: No updates recorded, skipping categorization" << std::endl;
        return;
    }
    
    // Step 4: Categorize using CDF
    std::vector<uint64_t> hot_nodes, warm_nodes, cold_nodes;
    uint64_t cumulative_accesses = 0;
    
    for (const auto& [lid, count] : updated_vertices) {
        // Add BEFORE checking CDF (so vertex contributes to its own category)
        cumulative_accesses += count;
        double cdf = static_cast<double>(cumulative_accesses) / total_accesses;
        
        // Check if vertex exists at vt
        db->begin_transaction();
        bool exists = db->try_get_node_at_vt(lid, vt).has_value();
        db->commit_transaction();
        
        if (!exists) continue;
        
        // Categorize: vertices whose cumulative contribution reaches threshold
        if (cdf < 0.3) {
            hot_nodes.push_back(lid);
        } else if (cdf < 0.6) {
            warm_nodes.push_back(lid);
        } else {
            cold_nodes.push_back(lid);
        }
    }   
    // Add never-updated vertices to cold (they exist but have 0 versions)
    for (uint64_t lid : active_nodes) {
        if (version_tracker[lid] == 0) {
            db->begin_transaction();
            bool exists = db->try_get_node_at_vt(lid, vt).has_value();
            db->commit_transaction();
            if (exists) {
                cold_nodes.push_back(lid);
            }
        }
    }
    

    // SAFE SAMPLING 
    std::vector<uint64_t> samples_q1_q3, samples_q2, samples_q4;
    while (samples_q1_q3.size() < SAMPLES_PER_ITER) {
        uint64_t id = active_nodes[query_zipf.next(local_gen) % active_nodes.size()];
        db->begin_transaction();
        bool ok = db->try_get_node_at_vt(id, vt).has_value();
        db->commit_transaction();
        if (ok) samples_q1_q3.push_back(id);
    }
    while (samples_q2.size() < SAMPLES_PER_ITER) {
        uint64_t id = active_nodes[query_zipf.next(local_gen) % active_nodes.size()];
        db->begin_transaction();
        bool ok = db->try_get_node_at_vt(id, start_window).has_value();
        db->commit_transaction();
        if (ok) samples_q2.push_back(id);
    }

    uint64_t target_q4 = (vt > 200000) ? (vt - 200000) : start_window;
    if (vt < 200000 && vt > 2000) target_q4 = 2000;

    int attempts = 0;
    while (samples_q4.size() < SAMPLES_PER_ITER && attempts++ < 10000) {
        uint64_t id = active_nodes[query_zipf.next(local_gen) % active_nodes.size()];
        db->begin_transaction();
        bool ok = db->try_get_node_at_vt(id, target_q4).has_value();
        db->commit_transaction();
        if (ok) samples_q4.push_back(id);
    }

    // MEASUREMENTS (Q1-Q4)
    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < samples_q1_q3.size(); ++i) {
        db->begin_transaction(); 
        query_ctx ctx(db);
        auto op = std::make_shared<poseidon::qop_as_of_scan>(vt, true);
        op->connect(sink, std::bind(&SilentPrinter::process, sink.get(), std::placeholders::_1, std::placeholders::_2), nullptr);
        qr_tuple t; t.push_back(samples_q1_q3[i]); op->process(ctx, t); 
        db->commit_transaction();
    }
    double q1 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - t1).count() / (1000.0 * samples_q1_q3.size());

    auto t3 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < samples_q2.size(); ++i) {
        db->begin_transaction(); 
        query_ctx ctx(db);
        auto op = std::make_shared<poseidon::qop_timeline_scan>(start_window, vt, true);
        op->connect(sink, std::bind(&SilentPrinter::process, sink.get(), std::placeholders::_1, std::placeholders::_2), nullptr);
        qr_tuple t; t.push_back(samples_q2[i]); op->process(ctx, t); 
        db->commit_transaction();
    }
    double q2 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - t3).count() / (1000.0 * samples_q2.size());

    auto t5 = std::chrono::high_resolution_clock::now();
    int q3_count = 0;
    for (size_t i = 0; i < samples_q1_q3.size(); ++i) {
        db->begin_transaction(); 
        auto opt_nid = db->try_get_node_at_vt(samples_q1_q3[i], vt);
        if (opt_nid.has_value()) {
            query_ctx ctx(db); 
            auto op = std::make_shared<poseidon::qop_temporal_expand>(vt, true, "");
            op->connect(sink, std::bind(&SilentPrinter::process, sink.get(), std::placeholders::_1, std::placeholders::_2), nullptr);
            qr_tuple t; t.push_back(db->get_node_description(opt_nid.value())); 
            op->process(ctx, t); q3_count++;
        }
        db->commit_transaction();
    }
    double q3 = (q3_count == 0) ? 0.0 : std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - t5).count() / (1000.0 * q3_count);

    auto t7 = std::chrono::high_resolution_clock::now();
    int q4_count = 0;
    for (size_t i = 0; i < samples_q4.size(); ++i) {
        db->begin_transaction(); 
        auto opt_nid = db->try_get_node_at_vt(samples_q4[i], start_window);
        if (opt_nid.has_value()) {
            query_ctx ctx(db); 
            auto op = std::make_shared<poseidon::qop_time_slice_expand>(start_window, vt, true, "");
            op->connect(sink, std::bind(&SilentPrinter::process, sink.get(), std::placeholders::_1, std::placeholders::_2), nullptr);
            qr_tuple t; t.push_back(db->get_node_description(opt_nid.value())); 
            op->process(ctx, t); q4_count++;
        }
        db->commit_transaction();
    }
    double q4 = (q4_count == 0) ? 0.0 : std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - t7).count() / (1000.0 * q4_count);

    // HOT/WARM/COLD VARIANT MEASUREMENT
    auto measure_q3_variant = [&](const std::vector<uint64_t>& pool) {
        if (pool.empty()) {
            return 0.0;
        }
        auto start = std::chrono::high_resolution_clock::now();
        int count = 0;
        
        // Sample with replacement
        std::uniform_int_distribution<size_t> pool_dist(0, pool.size() - 1);
        
        for (int i = 0; i < SAMPLES_PER_ITER; ++i) {
            uint64_t lid = pool[pool_dist(local_gen)];
            db->begin_transaction(); 
            auto opt_nid = db->try_get_node_at_vt(lid, vt);
            if (opt_nid.has_value()) {
                query_ctx ctx(db); 
                auto op = std::make_shared<poseidon::qop_temporal_expand>(vt, true, "");
                op->connect(sink, std::bind(&SilentPrinter::process, sink.get(), std::placeholders::_1, std::placeholders::_2), nullptr);
                qr_tuple t; t.push_back(db->get_node_description(opt_nid.value()));
                op->process(ctx, t); 
                count++;
            }
            db->commit_transaction();
        }
        return (count == 0) ? 0.0 : std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() / (1000.0 * count);
    };
    
    double hot_lat = measure_q3_variant(hot_nodes);
    double warm_lat = measure_q3_variant(warm_nodes);
    double cold_lat = measure_q3_variant(cold_nodes);
    
    std::ofstream(FIG_C_CSV, std::ios_base::app) << current_ops << "," << hot_lat << "," << warm_lat << "," << cold_lat << "\n";

    // TIME SLICE WINDOW MEASUREMENT 
    // ============================================================================
    std::ofstream fig_d(FIG_D_CSV, std::ios_base::app);
    for (auto window : d_windows) {
        if (vt < window + 2000) continue;
        
        auto start = std::chrono::high_resolution_clock::now();
        int valid_samples = 0;
        
        std::uniform_int_distribution<uint64_t> time_dist(2000, vt - window);
        
        for (size_t i = 0; i < SAMPLES_PER_ITER; ++i) {
            uint64_t random_start = time_dist(local_gen);
            uint64_t random_end = random_start + window;
            
            uint64_t lid = active_nodes[query_zipf.next(local_gen) % active_nodes.size()];
            
            db->begin_transaction();
            auto opt_nid = db->try_get_node_at_vt(lid, random_start);
            if (opt_nid.has_value()) {
                query_ctx ctx(db);
                auto op = std::make_shared<poseidon::qop_time_slice_expand>(random_start, random_end, true, "");
                op->connect(sink, std::bind(&SilentPrinter::process, sink.get(), std::placeholders::_1, std::placeholders::_2), nullptr);
                qr_tuple t; t.push_back(db->get_node_description(opt_nid.value()));
                op->process(ctx, t); 
                valid_samples++;
            }
            db->commit_transaction();
        }
        
        double lat = (valid_samples == 0) ? 0.0 : 
            std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count() / (1000.0 * valid_samples);
        fig_d << current_ops << "," << window/1000 << "," << lat << "\n";
    }

    double total_mb = get_total_logical_mb(db);
    std::cout << ">>> Stage (Ops: " << current_ops << ", Iter: " << iteration << ")" << std::endl;
    std::ofstream(CSV_FILE, std::ios_base::app) << iteration << "," << current_ops << "," << q1 << "," << q2 << "," << q3 << "," << q4 << "," << total_mb << "\n";
}

int main() {
    spdlog::set_level(spdlog::level::err);
    auto db_ptr = std::make_shared<graph_db>("poseidon_bench", ".", 0);
    std::string base = "/home/rayhan/poseidon_core/Temporal-data-T-MGBench/incremental/";

    std::ofstream write_file(WRITE_LATENCY_CSV);
    write_file << "Total_Ops,Avg_Write_Latency_ms,Total_Logical_MB\n";
    write_file.close();
    
    std::ofstream file(CSV_FILE);
    file << "Iteration,Ops_Count,Q1_ms,Q2_ms,Q3_ms,Q4_ms,Total_Logical_MB\n";
    file.close();

    std::ofstream cf(FIG_C_CSV); cf << "Ops_Count,Hot_ms,Warm_ms,Cold_ms\n";
    std::ofstream df(FIG_D_CSV); df << "Ops_Count,Window_Seconds,Q4_Latency_ms\n";

    std::cout << ">>> STAGE 1: Initial Load..." << std::endl;
    db_ptr->run_transaction([&](){
        load_finbench_nodes(*db_ptr, base + "AddPersonWrite1.csv");
        load_finbench_nodes(*db_ptr, base + "AddCompanyWrite2.csv");
        load_finbench_nodes(*db_ptr, base + "AddMediumWrite3.csv");
        load_finbench_edges(*db_ptr, base + "AddAccountTransferAccountReadWrite1.csv");
        return true;
    });

    // FIX 2: TEMPORAL VISIBILITY REGISTRATION
    // We must 'register' initial nodes in the temporal index so get_node_at_vt works
    std::vector<uint64_t> active_nodes;
    db_ptr->run_transaction([&](){
        auto raw_lids = db_ptr->get_active_node_lids();
        for (auto lid : raw_lids) {
            if (lid < 0x7FFFFFFF) {
                uint64_t phys = db_ptr->find_current_node(lid);
                if (phys != UNKNOWN) {
                    db_ptr->update_node_create_version(phys, {}, 2000); // Registers lid at VT=2000
                    active_nodes.push_back(lid);
                    version_tracker[lid] = 0;
                }
            }
        }
        return true;
    });

    auto raw_node_set = db_ptr->get_active_node_lids();
    //std::vector<uint64_t> active_nodes;
    for(auto lid : raw_node_set) {
        if (lid < 0x7FFFFFFF) {
            active_nodes.push_back(lid);
            version_tracker[lid] = 0; 
        }
    }
    
    std::default_random_engine gen(42);
    ZipfGenerator node_zipf(active_nodes.size(), 1.1);
    std::uniform_int_distribution<int> mix_dist(1, 100);

    int last_ops = 0;
    for (int target_ops : OPS_STAGES) {
        int ops_to_add = target_ops - last_ops;
        double total_duration_us = 0;

        for (int i = 0; i < ops_to_add; ++i) {
            int current_total_i = last_ops + i;
            uint64_t vt = 2000 + current_total_i + 1;           
            db_ptr->begin_transaction();
            int roll = mix_dist(gen);

            try {
                if (roll <= 80 && !active_nodes.empty()) { 
                    uint64_t lid = active_nodes[node_zipf.next(gen) % active_nodes.size()];                   
                    uint64_t phys_id = db_ptr->get_node_at_vt(lid, vt - 1);                                       
                    if (phys_id != UNKNOWN && phys_id != 0) {
                        auto sw = std::chrono::high_resolution_clock::now();
                        db_ptr->update_node_create_version(phys_id, {
                            {"balance", (double)(rand() % 10000) / 100.0},
                            {"locationIP", "192.168.1.1"},
                            {"browserUsed", "Chrome/110.0.0"}
                        }, vt);
                        auto end_time = std::chrono::high_resolution_clock::now();
                        total_duration_us += std::chrono::duration_cast<std::chrono::microseconds>(end_time - sw).count();
                        version_tracker[lid]++;
                    }
                } 
                else if (roll <= 90) { 
                    if (rand() % 2 == 0 || active_nodes.empty()) {
                        uint64_t new_lid = 2000000 + current_total_i;
                        auto start_time = std::chrono::high_resolution_clock::now();
                        db_ptr->add_node("person", {{"v", (double)vt}, {"balance", 0.0}}, vt, 1000000, new_lid);
                        auto end_time = std::chrono::high_resolution_clock::now();
                        total_duration_us += std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
                        active_nodes.push_back(new_lid);
                        version_tracker[new_lid] = 0;
                    } else {
                        uint64_t src = active_nodes[node_zipf.next(gen) % active_nodes.size()];
                        uint64_t dst = active_nodes[rand() % active_nodes.size()];
                        auto start_time = std::chrono::high_resolution_clock::now();
                        db_ptr->add_relationship(src, dst, "knows", {}, vt, 1000000, 70000000 + current_total_i);
                        auto end_time = std::chrono::high_resolution_clock::now();
                        total_duration_us += std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
                    }
                }
                else { 
                    if (active_nodes.size() > 100) {
                        uint64_t lid = active_nodes.back();                        
                        uint64_t phys_id = db_ptr->get_node_at_vt(lid, vt - 1);                                                                                     
                        if (phys_id != UNKNOWN && phys_id != 0) {
                            auto start_time = std::chrono::high_resolution_clock::now();
                            db_ptr->stream_rship_lids(lid, true, [&](uint64_t r_lid) {
                                uint64_t r_phys = db_ptr->find_current_rship(r_lid);
                                if (r_phys != UNKNOWN) db_ptr->close_relationship_at_vt(r_phys, vt);
                            });
                            db_ptr->stream_rship_lids(lid, false, [&](uint64_t r_lid) {
                                uint64_t r_phys = db_ptr->find_current_rship(r_lid);
                                if (r_phys != UNKNOWN) db_ptr->close_relationship_at_vt(r_phys, vt);
                            });
                            db_ptr->close_node_at_vt(phys_id, vt); 
                            auto end_time = std::chrono::high_resolution_clock::now();
                            total_duration_us += std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
                        }
                        active_nodes.pop_back();
                        //version_tracker.erase(lid);
                    }
                }
            } catch(...) { }
            db_ptr->commit_transaction();
            if (i % 40000 == 0 && i > 0) std::cout << "  ...batch progress: " << i << " ops" << std::endl;
        }

        double avg_write_latency_ms = (total_duration_us / 1000.0) / ops_to_add;
        double total_mb = get_total_logical_mb(db_ptr); 
        std::ofstream write_out(WRITE_LATENCY_CSV, std::ios_base::app);
        write_out << target_ops << "," << avg_write_latency_ms << "," << total_mb << "\n"; 
        write_out.close();

        std::cout << ">>> Batch complete. Waiting for background archival..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));

        uint64_t latest_vt = 2000 + target_ops;
        for (int iter = 1; iter <= BENCHMARK_ITERATIONS_PER_STAGE; ++iter) {
            run_bench_stage(db_ptr, active_nodes, latest_vt, iter, target_ops);
        }
        last_ops = target_ops;
    }
    return 0;
}