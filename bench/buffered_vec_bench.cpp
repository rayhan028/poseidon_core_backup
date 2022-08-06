#include "config.h"
#include "defs.hpp"
#include "graph_db.hpp"
#include "nodes.hpp"

#include "spdlog/spdlog.h"

void create_data(p_ptr<node_list<buffered_vec> > nlist) {
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

p_ptr<node_list<buffered_vec> > prepare_table(bufferpool& bp) {
  auto nlist = p_make_ptr<node_list<buffered_vec> >(bp, 0);
  create_data(nlist);
  return nlist;
}

void drop_table(p_ptr<node_list<buffered_vec> > nlist) {
  unsigned long num = 0;
  for (__attribute__((unused)) auto &n : nlist->as_vec()) {
    num++;
  }
  std::cout << num << " records found (expected: 109'000)" << std::endl;

  // delete nlist;
}

int main(int argc, char **argv) {
  std::cout << "sizeof = " << sizeof(txn<dirty_node_ptr>) << std::endl;
  // add node
  {
    auto test_file = std::make_shared<paged_file>();
    test_file->open("nodes.db", 0);

    bufferpool bpool;
    bpool.register_file(0, test_file);

    auto nlist = prepare_table(bpool);
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
  remove("nodes.db");

  // append node
  {
    auto test_file = std::make_shared<paged_file>();
    test_file->open("nodes.db", 0);

    bufferpool bpool;
    bpool.register_file(0, test_file);

    auto nlist = prepare_table(bpool);
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
  remove("nodes.db");
  // store node
  {
    auto test_file = std::make_shared<paged_file>();
    test_file->open("nodes.db", 0);

    bufferpool bpool;
    bpool.register_file(0, test_file);

    auto nlist = prepare_table(bpool);
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
  remove("nodes.db");
}