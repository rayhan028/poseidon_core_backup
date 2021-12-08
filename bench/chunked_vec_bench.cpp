#include "config.h"
#include "defs.hpp"
#include "graph_db.hpp"
#include "nodes.hpp"

#include "spdlog/spdlog.h"

#ifdef USE_PMDK

#define PMEMOBJ_POOL_SIZE                                                      \
  ((unsigned long long)(1024 * 1024 * 40000ull)) // 4000 MiB

namespace nvm = pmem::obj;
const std::string bench_path = poseidon::gPmemPath + "bench";

struct root {
  pmem::obj::persistent_ptr<node_list> nlist_p;
};

nvm::pool<root> pop;
#endif

void create_data(p_ptr<node_list> nlist) {
  // 45 nodes per chunk
  for (auto i = 0lu; i < 100'000; i++) {
    nlist->append(node(i));
  }
  std::cout << nlist->num_chunks() << " chunks created." << std::endl;
  std::vector<offset_t> victims(1000);
  offset_t idx = 20;
  for (int i = 0u; i < 1000; i += 2) {
    victims[i] = idx;
    victims[i + 1] = idx + 2;
    victims[i + 4] = idx + 4;
    idx += 45;
  }
  for (auto &v : victims) {
    nlist->remove(v);
  }

  unsigned long num = 0;
  for (__attribute__((unused)) auto &n : nlist->as_vec()) {
    num++;
  }
  assert(num == 99'000);
}

#ifdef USE_PMDK

p_ptr<node_list> prepare_table() {
  pop = nvm::pool<root>::create(bench_path, "", PMEMOBJ_POOL_SIZE);
  auto root_obj = pop.root();

  nvm::transaction::run(
      pop, [&] { root_obj->nlist_p = nvm::make_persistent<node_list>(); });

  auto nlist = root_obj->nlist_p;
  create_data(nlist);
  return nlist;
}

void drop_table(p_ptr<node_list> nlist) {
  unsigned long num = 0;
  for (auto &n : nlist->as_vec()) {
    num++;
  }
  std::cout << num << " records found (expected: 109'000)" << std::endl;

  //nvm::transaction::run(
  //    pop, [&] { nvm::delete_persistent<node_list>(pop.root()->nlist_p); });

  pop.close();
  remove(bench_path.c_str());
  std::cout << "table dropped." << std::endl;
}
#else
p_ptr<node_list> prepare_table() {
  auto nlist = p_make_ptr<node_list>();
  create_data(nlist);
  return nlist;
}

void drop_table(p_ptr<node_list> nlist) {
  unsigned long num = 0;
  for (__attribute__((unused)) auto &n : nlist->as_vec()) {
    num++;
  }
  std::cout << num << " records found (expected: 109'000)" << std::endl;

  // delete nlist;
}
#endif

int main(int argc, char **argv) {
  std::cout << "sizeof = " << sizeof(txn<dirty_node_ptr>) << std::endl;
  // add node
  {
    auto nlist = prepare_table();
    std::cout << "starting add..." << std::endl;
    auto start_qp = std::chrono::steady_clock::now();

    for (auto i = 0lu; i < 10'000; i++) {
      nlist->add(node(i));
    }

    auto end_qp = std::chrono::steady_clock::now();
    auto tm =
        std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp)
            .count();
    std::cout << "add: " << tm << " microseconds." << std::endl;
    drop_table(nlist);
  }

  // append node
  {
    auto nlist = prepare_table();
    std::cout << "starting append..." << std::endl;
    auto start_qp = std::chrono::steady_clock::now();

    for (auto i = 0lu; i < 10'000; i++) {
      nlist->append(node(i));
    }

    auto end_qp = std::chrono::steady_clock::now();
    auto tm =
        std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp)
            .count();
    std::cout << "append: " << tm << " microseconds." << std::endl;
    drop_table(nlist);
  }
  // store node
  {
    auto nlist = prepare_table();
    std::cout << "starting insert..." << std::endl;
    auto start_qp = std::chrono::steady_clock::now();

    for (auto i = 0lu; i < 10'000; i++) {
      nlist->insert(node(i));
    }

    auto end_qp = std::chrono::steady_clock::now();
    auto tm =
        std::chrono::duration_cast<std::chrono::microseconds>(end_qp - start_qp)
            .count();
    std::cout << "store: " << tm << " microseconds." << std::endl;
    drop_table(nlist);
  }
}
