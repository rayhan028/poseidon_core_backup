#include <iostream>
#include <regex>
#include <random>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "defs.hpp"
#include "gtpc.hpp"
#include "config.h"
#include "graph_pool.hpp"
#include "graphgen/generator.hpp"

#include "threadsafe_queue.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

using namespace boost::program_options;

int main(int argc, char **argv) {
  bool strict = false;
  std::string directory;
  std::size_t warehouses;

  try {
    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Help")
        ("verbose,v", bool_switch()->default_value(false), "Verbose - show debug output")
        ("directory,d", value<std::string>(&directory)->required(), "Path to out directory for generated GTPC CSV files (required)")
        ("warehouses,w", value<std::size_t>(&warehouses)->required(),"Number of warehouse (required)");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << "Poseidon Graph Database GTPC Benchmark Version " << POSEIDON_VERSION
                << "\n"
                << desc << '\n';
      return -1;
    }
    if (vm.count("directory"))
      directory = vm["directory"].as<std::string>();

    if (vm.count("warehouses"))
      warehouses = vm["warehouses"].as<std::size_t>();

    notify(vm);

  } catch (const error &ex) {
    std::cerr << ex.what() << '\n';
    return -1;
  }

  std::string wstr = (warehouses > 1) ? "warehouses" : "warehouse";
  spdlog::info("--------- Generating GTPC data with {} {}", warehouses, wstr);
  auto start = std::chrono::steady_clock::now();

  GtpcGenerator generator((uint32_t)warehouses, directory);
  generator.generateWarehouses();
  generator.generateDistricts();
  generator.generateCustomerAndHistory();
  generator.generateItems();
  generator.generateSuppliers();
  generator.generateStock();
  generator.generateOrdersAndOrderLines();
  generator.generateRegions();
  generator.generateNations();

  auto end = std::chrono::steady_clock::now();
  double t = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  spdlog::info("--------- Data generation completed in {} msecs:", t);

  return 0;
}
