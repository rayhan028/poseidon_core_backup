#include <dpu>
#include "graph_db.hpp"

using namespace dpu;

void dpu_scan(graph_db_ptr &gdb) {
    try {
        // TODO: 1. upload chunks to MRAM
        // TODO: 2. allocate DPUs
        auto dpu = DpuSet::allocate(1);
        dpu.load("helloworld");

        auto start = std::chrono::steady_clock::now();

        // 3. execute the scan
        dpu.exec();

        auto end = std::chrono::steady_clock::now();

        std::chrono::duration<double> diff = end - start;
        std::cout << "Scan executed in " << diff.count() << " secs" << std::endl;
        dpu.log(std::cout);
    }
    catch (const DpuError & e) {
        std::cerr << e.what() << std::endl;
    }
}
