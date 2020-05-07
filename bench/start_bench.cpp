#include "config.h"
#include "defs.hpp"
#include "graph_db.hpp"
#include "graph_pool.hpp"

#include <chrono>
#include <boost/program_options.hpp>

#include "spdlog/spdlog.h"

using namespace boost::program_options;

int main(int argc, char **argv) {
    std::string pool_path, graph_name;

 try {       
    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Help")
        ("pool,p", value<std::string>(&pool_path)->required(), "Path to the PMem pool")
        ("db,d", value<std::string>(&graph_name)->required(),"Database name (required)");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << "Poseidon Start benchmark\n"
                << desc << '\n';
      return -1;
    }
    if (vm.count("pool"))
      pool_path = vm["pool"].as<std::string>();

    if (vm.count("graph_name"))
      graph_name = vm["graph_name"].as<std::string>();

    notify(vm);

      } catch (const error &ex) {
    std::cerr << ex.what() << '\n';
    return -1;
  }
 
    spdlog::info("try to open {} with layout '{}'", pool_path, graph_name);

    auto start_qp = std::chrono::steady_clock::now();

    auto pool = graph_pool::open(pool_path, graph_name);
    auto graph = pool->open_graph(graph_name);
 
    auto end_qp = std::chrono::steady_clock::now();
    auto tm =
        std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp)
            .count();
    std::cout << "startup: " << tm << " microseconds." << std::endl;
}
