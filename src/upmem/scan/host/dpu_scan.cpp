#include <dpu>
#include <dpu_management.h>
#include "graph_db.hpp"
#include <map>
#include <dpu_config.h>
#include "../common/defs.h"
#include <fstream>
#include "thread_pool.hpp"

using namespace dpu;

#define BUFFER_SIZE (1 << 16)


std::vector<dpu_set_t*> assigned_dpus;



void assign_chunks_to_dpus(dpu_set_t &set, graph_db_ptr &gdb, uint32_t ndpus) {
    auto & nodes = gdb->get_nodes();

    auto iter = nodes->as_vec().chunk_list_begin();

    int dpu_id = 0;
    int assigned_chunks = 0;
    std::map<int,uint64_t> assigned_chunks_dpu;

    struct dpu_set_t rank, dpu;

    int o = 0;
    while(iter != nodes->as_vec().chunk_list_end()) {
        DPU_RANK_FOREACH(set, rank) {
            DPU_FOREACH(rank, dpu, dpu_id) {
                if(assigned_chunks_dpu.find(dpu_id) == assigned_chunks_dpu.end()) {
                    assigned_chunks_dpu[dpu_id] = 0;
                }
                
                //DPU_ASSERT(dpu_copy_to(dpu, "mr_chunk", o*nodes->as_vec().real_chunk_size(), *iter, nodes->as_vec().real_chunk_size()));
                DPU_ASSERT(dpu_prepare_xfer(dpu, *iter));
                //DPU_ASSERT(dpu_launch(dpu, DPU_ASYNCHRONOUS));
                assigned_dpus.push_back(&dpu);
                assigned_chunks_dpu[dpu_id]++;
                assigned_chunks++;
                iter++;    
                if(iter == nodes->as_vec().chunk_list_end()) break;
            }
            dpu_push_xfer(rank, DPU_XFER_TO_DPU, "mr_chunk", o*nodes->as_vec().real_chunk_size(), nodes->as_vec().real_chunk_size(), DPU_XFER_DEFAULT);
            if(iter == nodes->as_vec().chunk_list_end()) break;
        }       
        o++;
    }
    
    DPU_RANK_FOREACH(set, rank) {
        DPU_FOREACH(rank, dpu, dpu_id) { 
            DPU_ASSERT(dpu_prepare_xfer(dpu, &assigned_chunks_dpu[dpu_id]));
        }

        DPU_ASSERT(dpu_push_xfer(rank, DPU_XFER_TO_DPU, "assigned_chunks", 0, sizeof(uint64_t), DPU_XFER_DEFAULT));
        DPU_ASSERT(dpu_sync(rank));
    }
}

std::vector<std::vector<mrchunk*>> assign_chunks_to_threads(graph_db_ptr &gdb, unsigned int nthreads) {
    auto & nodes = gdb->get_nodes();
    auto iter = nodes->as_vec().chunk_list_begin();

    std::vector<std::vector<mrchunk*>> chunk_ptrs(nthreads);
    
    while(iter != nodes->as_vec().chunk_list_end()) {

    }
}

int results[32];
void baseline_scan(graph_db_ptr gdb, std::size_t first, std::size_t last, int tid) {
    auto iter = gdb->get_nodes()->range(first, last);
    while (iter) {
	  auto &n = *iter;
      if(n.node_label == 52)
	    results[tid]++;
	  ++iter;
  }
}
struct scan_task {
    scan_task(graph_db_ptr gdb, std::size_t first, std::size_t last, int tid) : gdb_(gdb), first_(first), last_(last), tid_(tid) {}
    
    void operator()() {
        baseline_scan(gdb_,first_,last_,tid_);
    }

    graph_db_ptr gdb_; 
    std::size_t first_;
    std::size_t last_;
    int tid_;
};

void parallel_scan(graph_db_ptr &gdb_, int threads) {
  std::vector<std::future<void>> res;
  thread_pool pool(threads);

  const int nchunks = 1;
  //std::cout << "Start parallel query with threads" << 
  //              gdb_->get_nodes()->num_chunks() / nchunks + 1 << std::endl;;

  res.reserve(gdb_->get_nodes()->num_chunks() / nchunks + 1);
  std::size_t start = 0, end = nchunks - 1;
  int i = 0;
  while (start < gdb_->get_nodes()->num_chunks()) {
    res.push_back(pool.submit(
        scan_task(gdb_, start, end, i)));
    start = end + 1;
    end += nchunks;
  }
 
  // std::cout << "waiting ..." << std::endl;
  for (auto &f : res) {
    f.get();
  }
}
#include <numeric>

void dpu_scan(graph_db_ptr &gdb) {
    std::ofstream baseline_result_file;
    baseline_result_file.open("baseline_results.csv");
    
    for(int i = 1; i <= std::thread::hardware_concurrency(); i++) {
        std::vector<int> times;
        for(int k = 0; k < 100; k++) {
            auto start_scan = std::chrono::steady_clock::now();
            parallel_scan(gdb,i);
            auto end_scan = std::chrono::steady_clock::now();
            for(int j = 0; j < 32; j++) results[j] = 0;
            int duration = std::chrono::duration_cast<std::chrono::microseconds>(end_scan-start_scan).count();
            times.push_back(duration);
        }
        int duration = (int)std::accumulate( times.begin(), times.end(), 0.0)/times.size();  
        std::cout << "Baseline " << i << "threads: " 
        << duration << " µsecs" << std::endl;
        baseline_result_file << duration;
        if(i!=std::thread::hardware_concurrency()) baseline_result_file << ",";
    }
    baseline_result_file << "\n";

    auto& nodes = gdb->get_nodes();
    {
        node n;
        std::cout << "chunk_size for nodes: " << nodes->as_vec().real_chunk_size() << "\n"
                  << "elements per chunk: " << nodes->as_vec().elements_per_chunk() << "\n"
                  << "number of chunks    : " << nodes->as_vec().num_chunks() << "\n"
                  << "size of a node      : " << sizeof(node) << "\n"
                  << "offset of id_       : " << n._offset() << std::endl; 
    }
    std::ofstream result_file;
    result_file.open("upmem_results.csv");
    try {
        for(int j = 2; j <= 510; j++) {
            std::vector<int> tasklet_durations;
            uint32_t nr_dpus;
            uint32_t nr_ranks;
            for(int i = 1; i < 25; i++) {
                struct dpu_set_t set, dpu, rank;
                // TODO: 1. allocate DPUs 
                DPU_ASSERT(dpu_alloc(j, NULL, &set));
                DPU_ASSERT(dpu_get_nr_dpus(set, &nr_dpus));
                DPU_ASSERT(dpu_get_nr_ranks(set, &nr_ranks));
                //std::cout << "#dpus = " << nr_dpus << std::endl;
                //std::cout << "#ranks = " << nr_ranks << std::endl;
                std::string dpu_program = "mram_scan" + std::to_string(i);
                DPU_ASSERT(dpu_load(set, dpu_program.c_str(), NULL));

                

                // 2. upload chunks to MRAM
                auto start_transfer = std::chrono::steady_clock::now();
                assign_chunks_to_dpus(set, gdb, nr_dpus);
                auto end_transfer = std::chrono::steady_clock::now();
                std::chrono::duration<double> diff_transfer = end_transfer - start_transfer;
                //std::cout << "Transfer executed in " << std::chrono::duration_cast<std::chrono::microseconds>(diff_transfer).count() << " µsecs" << std::endl;
            
                // 3. execute the scan
                auto start = std::chrono::steady_clock::now();
                dpu_launch(set, DPU_SYNCHRONOUS);
                auto end = std::chrono::steady_clock::now();
                
                std::chrono::duration<double> diff = end - start;

                //printf("printing log for dpu:\n");
                //DPU_FOREACH(set, dpu) {
                    //dpu_log_read(dpu, stdout);
                //}

                
                // Copy results
                uint64_t found_results = 0;
                uint64_t tmp[i];
                DPU_RANK_FOREACH(set, rank) {

                    DPU_FOREACH(rank, dpu) {
                        DPU_ASSERT(dpu_prepare_xfer(dpu, &tmp));
                        dpu_push_xfer(dpu, DPU_XFER_FROM_DPU, "found_results", 0, sizeof(uint64_t) * i, DPU_XFER_DEFAULT);
                        //dpu_copy_from(dpu, "found_results", 0, &tmp, sizeof(uint64_t));
                        for(int i = 0; i < i; i++) {
                            found_results += tmp[i];
                            tmp[i] = 0;
                        }
                    }
                }
                
                //std::cout << "MRAMScan results: " << found_results << std::endl;
                int duration = std::chrono::duration_cast<std::chrono::microseconds>(diff).count();
                std::cout << "MRAM Scan DPUs: " << nr_dpus << " Tasklets: " << i << " in " << duration << " µsecs" << std::endl;
                tasklet_durations.push_back(duration);
                DPU_ASSERT(dpu_free(set));
            }
            result_file << nr_dpus << ",";
            for(auto d : tasklet_durations) {
                result_file << d << ",";
            }
            result_file << "\n";
        }

        /*auto start_scan = std::chrono::steady_clock::now();
        int resp = 0;
        int results = 0;
        for(auto & n : nodes->as_vec()) {
            if(n.node_label == 52) {
                results++;
            }
        }

        auto end_scan = std::chrono::steady_clock::now();

        std::cout << "NodeScan results: " << results << std::endl;
        std::chrono::duration<double> diff_scan = end_scan - start_scan;
        std::cout << "NodeScan executed in " << std::chrono::duration_cast<std::chrono::microseconds>(diff_scan).count() << " µsecs" << std::endl;
        std::cout << sizeof(mrchunk ) << std::endl;*/
    }
    catch (const DpuError & e) {
        std::cerr << e.what() << std::endl;
    }
    result_file.close();
}

/* 

int offset = 0;
        std::cout << "DPU " << dpu_id++ << " :";
        // assign #chunks_per_dpu to _dpu
        //auto _chunks = dpu_id == ndpus ? (nr_chunks - assigned_chunks) : chunks_per_dpu;
        for(int i = 0; i < chunks_per_dpu; i++) {
            mr_node* chk_ptr = (mr_node*)*iter;
            dpu_copy_to(_dpu, "mr_chunk", offset, chk_ptr, nodes->as_vec().real_chunk_size()); 
            offset += 65536 * sizeof(mr_node);  
            iter++; 
            std::cout << " " << assigned_chunks++;
        }
        std::cout << std::endl;

*/