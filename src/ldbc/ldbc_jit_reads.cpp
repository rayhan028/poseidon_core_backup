#include <iostream>
#include <numeric>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "defs.hpp"
#include "graph_db.hpp"
#include "graph_pool.hpp"
#include "ldbc.hpp"
#include "config.h"

#include "threadsafe_queue.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#include "ldbc_jit_reads.hpp"
#include "ldbc.hpp"

using namespace boost::program_options;
bool compiled = false;
std::vector<int64_t> runtimes;

double calc_avg() {
  auto avg = std::accumulate(runtimes.begin(), runtimes.end(), 0) / runtimes.size();
  runtimes.clear();
  compiled = false;
  scan_task::callee_ = &scan_task::scan;
  return avg;
}

void ldbc_jit_is_query_1_a(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t personId) {
  auto q = Scan("Person",
              Filter(EQ(Key(0, "id"), Int(personId)),
                ForeachRship(RSHIP_DIR::FROM, {}, ":isLocatedIn", 
                  Expand(EXPAND::OUT, "Place",
                    Project({{0, "firstName", FTYPE::STRING}, {0, "lastName", FTYPE::STRING},
                            {0, "birthday", FTYPE::DATE}, {0, "locationIP", FTYPE::STRING},
                            {0, "browserUsed", FTYPE::STRING}, {2, "id", FTYPE::UINT64},
                            {0, "gender", FTYPE::STRING}, {0, "creationDate", FTYPE::TIME}}, 
                      Collect())))));
  arg_builder ab;
  ab.arg(1, "Person");
  ab.arg(2, personId);
  ab.arg(3, ":isLocatedIn");
  ab.arg(4, "Place");

  if(!compiled) {
    auto c_s = std::chrono::steady_clock::now();
    qeng.generate(q, adaptive);
    compiled = true;
    auto c_e = std::chrono::steady_clock::now();
      std::cout << "Compilation: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(c_e-c_s).count()
        << " ms" << std::endl;
  }

  auto e_s = std::chrono::steady_clock::now();
  gdb->begin_transaction();
  qeng.run_parallel(&rs, ab, 24);
  gdb->commit_transaction();
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();

  std::cout << "Execution: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count()
        << " ms" << std::endl;
  runtimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count());
}

void ldbc_jit_is_query_1_b(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t personId) {
  auto q = Scan("Place",
            ForeachRship(RSHIP_DIR::TO, {}, ":isLocatedIn",
              Expand(EXPAND::IN, "Person",
                Filter(EQ(Key(2, "id"), Int(personId)),
                  Project({{0, "firstName", FTYPE::STRING}, {0, "lastName", FTYPE::STRING},
                            {0, "birthday", FTYPE::DATE}, {0, "locationIP", FTYPE::STRING},
                            {0, "browserUsed", FTYPE::STRING}, {2, "id", FTYPE::STRING},
                            {0, "gender", FTYPE::STRING}, {0, "creationDate", FTYPE::TIME}}, 
                    Collect())))));

  arg_builder ab;
  ab.arg(1, "Place");
  ab.arg(2, ":isLocatedIn");
  ab.arg(3, ":Person");
  ab.arg(4, personId);

  qeng.generate(q, adaptive);
  qeng.run(&rs, ab.args);
}

void ldbc_jit_is_query_2_p(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t personId) {
  auto sort_fct = [&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[1]) == boost::get<boost::posix_time::ptime>(qr2[1]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[1]) > boost::get<boost::posix_time::ptime>(qr2[1]); };

  auto q = Scan("Person", 
            Filter(EQ(Key(0, "id"), Int(personId)), 
              ForeachRship(RSHIP_DIR::TO, {}, ":hasCreator",
                Expand(EXPAND::IN, "Post",
                  Limit(10, 
                    Project({{2, "id", FTYPE::UINT64}, /*{2, "content", FTYPE::STRING},*/
                           {2, "creationDate", FTYPE::TIME}, {2, "id", FTYPE::UINT64},
                           {0, "id", FTYPE::UINT64}, {0, "firstName", FTYPE::STRING},
                           {0, "lastName", FTYPE::STRING}}, 
                    /*Sort(sort_fct,*/
                    Collect()))))));
  
  arg_builder ab;
  ab.arg(1, "Person");
  ab.arg(2, personId);
  ab.arg(3, ":hasCreator");
  ab.arg(4, "Post");

  if(!compiled) {
    auto c_s = std::chrono::steady_clock::now();
    qeng.generate(q, adaptive);
    compiled = true;
    auto c_e = std::chrono::steady_clock::now();
      std::cout << "Compilation: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(c_e-c_s).count()
        << " ms" << std::endl;
  }

  auto e_s = std::chrono::steady_clock::now();
  gdb->begin_transaction();
  qeng.run_parallel(&rs, ab, 24);
  gdb->commit_transaction();
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();

  std::cout << "Execution: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count()
        << " ms" << std::endl;
  runtimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count());
}

void ldbc_jit_is_query_2_c(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t personId) {
  auto maxHops = 100;
  
  auto q  = Scan("Person",
              Filter(EQ(Key(0, "id"), Int(personId)), 
                ForeachRship(RSHIP_DIR::TO, {}, ":hasCreator",
                  Expand(EXPAND::IN, "Comment", 
                    ForeachRship(RSHIP_DIR::FROM, {1, maxHops}, ":replyOf",
                      Expand(EXPAND::OUT, "Post",
                        ForeachRship(RSHIP_DIR::FROM, {}, ":hasCreator",
                          Expand(EXPAND::OUT, "Person",
                            Limit(10, 
                              Project({{2, "id", FTYPE::UINT64}, {2, "content", FTYPE::STRING},
                                      {2, "creationDate", FTYPE::TIME}, {4, "id", FTYPE::UINT64},
                                      {6, "id", FTYPE::UINT64}, {6, "firstName", FTYPE::STRING},
                                      {6, "lastName", FTYPE::STRING}}, 
                                /*Sort([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                                  if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                                    return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                                  return boost::get<boost::posix_time::ptime>(qr1[2]) > boost::get<boost::posix_time::ptime>(qr2[2]); }, */ 
                                  Collect()))))))))));

  arg_builder ab;
  ab.arg(1, "Person");
  ab.arg(2, personId);
  ab.arg(3, ":hasCreator");
  ab.arg(4, "Comment");
  ab.arg(5, ":replyOf");
  ab.arg(6, "Post");
  ab.arg(7, ":hasCreator");
  ab.arg(8, "Person");

  if(!compiled) {
    auto c_s = std::chrono::steady_clock::now();
    qeng.generate(q, adaptive);
    compiled = true;
    auto c_e = std::chrono::steady_clock::now();
      std::cout << "Compilation: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(c_e-c_s).count()
        << " ms" << std::endl;
  }

  auto e_s = std::chrono::steady_clock::now();
  gdb->begin_transaction();
  qeng.run_parallel(&rs, ab, 24);
  gdb->commit_transaction();
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();

  std::cout << "Execution: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count()
        << " ms" << std::endl;
  runtimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count());
}

void ldbc_jit_is_query_3(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t personId) {
  auto q = Scan("Person",
            Filter(EQ(Key(0, "id"), Int(personId)), 
              ForeachRship(RSHIP_DIR::FROM, {}, ":knows",
                Expand(EXPAND::OUT, "Person",
                  Project({{2, "id", FTYPE::UINT64}, {2, "firstName", FTYPE::STRING},
                           {2, "lastName", FTYPE::STRING}, {1, "creationDate", FTYPE::TIME}}, 
                    /*Sort([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                          if (boost::get<boost::posix_time::ptime>(qr1[3]) == boost::get<boost::posix_time::ptime>(qr2[3]))
                            return boost::get<uint64_t>(qr1[0]) < boost::get<uint64_t>(qr2[0]);
                          return boost::get<boost::posix_time::ptime>(qr1[3]) > boost::get<boost::posix_time::ptime>(qr2[3]); },*/
                      Collect())))));

  arg_builder ab;
  ab.arg(1, "Person");
  ab.arg(2, personId);
  ab.arg(3, ":knows");
  ab.arg(4, "Person");

  if(!compiled) {
    auto c_s = std::chrono::steady_clock::now();
    qeng.generate(q, adaptive);
    compiled = true;
    auto c_e = std::chrono::steady_clock::now();
      std::cout << "Compilation: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(c_e-c_s).count()
        << " ms" << std::endl;
  }
/*
  auto e_s = std::chrono::steady_clock::now();
  qeng.run(&rs, ab.args, false);
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();*/
  auto e_s = std::chrono::steady_clock::now();
  gdb->begin_transaction();
  qeng.run_parallel(&rs, ab, 24);
  gdb->commit_transaction();
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();

  std::cout << "Execution: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count()
        << " ms" << std::endl;
  runtimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count());
}

void ldbc_jit_is_query_4_p(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t messageId) {
  auto q = Scan("Post",
            Filter(EQ(Key(0, "id"), Int(messageId)), 
              Project({{0, "creationDate", FTYPE::TIME}, /*{0, "imageFile", FTYPE::STRING}*/}, 
                Collect())));
  
  arg_builder ab;
  ab.arg(1, "Post");
  ab.arg(2, messageId);

  if(!compiled) {
    auto c_s = std::chrono::steady_clock::now();
    qeng.generate(q, adaptive);
    compiled = true;
    auto c_e = std::chrono::steady_clock::now();
      std::cout << "Compilation: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(c_e-c_s).count()
        << " ms" << std::endl;
  }
/*
  auto e_s = std::chrono::steady_clock::now();
  qeng.run(&rs, ab.args, false);
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();
*/
  auto e_s = std::chrono::steady_clock::now();
  gdb->begin_transaction();
  qeng.run_parallel(&rs, ab, 24);
  gdb->commit_transaction();
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();

  std::cout << "Execution: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count()
        << " ms" << std::endl;
  runtimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count());
}

void ldbc_jit_is_query_4_c(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t messageId) {
  auto q = Scan("Comment",
            Filter(EQ(Key(0, "id"), Int(messageId)), 
              Project({{0, "creationDate", FTYPE::TIME}, {0, "content", FTYPE::STRING}}, 
                Collect())));
  
  arg_builder ab;
  ab.arg(1, "Comment");
  ab.arg(2, messageId);

  if(!compiled) {
    auto c_s = std::chrono::steady_clock::now();
    qeng.generate(q, adaptive);
    compiled = true;
    auto c_e = std::chrono::steady_clock::now();
      std::cout << "Compilation: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(c_e-c_s).count()
        << " ms" << std::endl;
  }
/*
  auto e_s = std::chrono::steady_clock::now();
  qeng.run(&rs, ab.args, false);
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();
*/
  auto e_s = std::chrono::steady_clock::now();
  gdb->begin_transaction();
  qeng.run_parallel(&rs, ab, 24);
  gdb->commit_transaction();
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();

  std::cout << "Execution: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count()
        << " ms" << std::endl;
  runtimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count());
}

void ldbc_jit_is_query_5_p(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t messageId) {
  auto q = Scan("Post",
            Filter(EQ(Key(0, "id"), Int(messageId)),
              ForeachRship(RSHIP_DIR::FROM, {}, ":hasCreator",
                Expand(EXPAND::OUT, "Person",
                  Project({{2, "id", FTYPE::UINT64}, {2, "firstName", FTYPE::STRING}, {2, "lastName", FTYPE::STRING}}, 
                    Collect())))));

  arg_builder ab;
  ab.arg(1, "Post");
  ab.arg(2, messageId);
  ab.arg(3, ":hasCreator");
  ab.arg(4, "Person");

  if(!compiled) {
    auto c_s = std::chrono::steady_clock::now();
    qeng.generate(q, adaptive);
    compiled = true;
    auto c_e = std::chrono::steady_clock::now();
      std::cout << "Compilation: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(c_e-c_s).count()
        << " ms" << std::endl;
  }
/*
  auto e_s = std::chrono::steady_clock::now();
  qeng.run(&rs, ab.args, false);
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();
*/
  auto e_s = std::chrono::steady_clock::now();
  gdb->begin_transaction();
  qeng.run_parallel(&rs, ab, 24);
  gdb->commit_transaction();
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();

  std::cout << "Execution: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count()
        << " ms" << std::endl;
  runtimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count());
}

void ldbc_jit_is_query_5_c(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t messageId) {
  auto q = Scan("Comment",
            Filter(EQ(Key(0, "id"), Int(messageId)),
              ForeachRship(RSHIP_DIR::FROM, {}, ":hasCreator",
                Expand(EXPAND::OUT, "Person",
                  Project({{2, "id", FTYPE::UINT64}, {2, "firstName", FTYPE::STRING}, {2, "lastName", FTYPE::STRING}}, 
                    Collect())))));

  arg_builder ab;
  ab.arg(1, "Comment");
  ab.arg(2, messageId);
  ab.arg(3, ":hasCreator");
  ab.arg(4, "Person");

  if(!compiled) {
    auto c_s = std::chrono::steady_clock::now();
    qeng.generate(q, adaptive);
    compiled = true;
    auto c_e = std::chrono::steady_clock::now();
      std::cout << "Compilation: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(c_e-c_s).count()
        << " ms" << std::endl;
  }
/*
  auto e_s = std::chrono::steady_clock::now();
  qeng.run(&rs, ab.args, false);
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();
*/
  auto e_s = std::chrono::steady_clock::now();
  gdb->begin_transaction();
  qeng.run_parallel(&rs, ab, 24);
  gdb->commit_transaction();
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();

  std::cout << "Execution: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count()
        << " ms" << std::endl;
  runtimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count());
}

void ldbc_jit_is_query_6_p(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t messageId) {
  auto q = Scan("Post",
            Filter(EQ(Key(0, "id"), Int(messageId)),
              ForeachRship(RSHIP_DIR::TO, {}, ":containerOf",
                Expand(EXPAND::IN, "Forum",
                  ForeachRship(RSHIP_DIR::FROM, {}, ":hasModerator",
                    Expand(EXPAND::OUT, "Person",
                      Project({{2, "id", FTYPE::UINT64}, {2, "title", FTYPE::STRING}, {4, "id", FTYPE::UINT64},
                               {4, "firstName", FTYPE::STRING}, {4, "lastName", FTYPE::STRING}}, 
                        Collect())))))));

  arg_builder ab;
  ab.arg(1, "Post");
  ab.arg(2, messageId);
  ab.arg(3, ":containerOf");
  ab.arg(4, "Forum");
  ab.arg(5, ":hasModerator");
  ab.arg(6, "Person");

  if(!compiled) {
    auto c_s = std::chrono::steady_clock::now();
    qeng.generate(q, adaptive);
    compiled = true;
    auto c_e = std::chrono::steady_clock::now();
      std::cout << "Compilation: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(c_e-c_s).count()
        << " ms" << std::endl;
  }
/*
  auto e_s = std::chrono::steady_clock::now();
  qeng.run(&rs, ab.args, false);
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();
*/
  auto e_s = std::chrono::steady_clock::now();
  gdb->begin_transaction();
  qeng.run_parallel(&rs, ab, 24);
  gdb->commit_transaction();
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();

  std::cout << "Execution: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count()
        << " ms" << std::endl;
  runtimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count());
}

void ldbc_jit_is_query_6_c(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t messageId) {
  auto maxHops = 100;

  auto q = Scan("Comment",
            Filter(EQ(Key(0, "id"), Int(messageId)),
              ForeachRship(RSHIP_DIR::FROM, {1, maxHops}, ":replyOf",
                Expand(EXPAND::OUT, "Post",
                  ForeachRship(RSHIP_DIR::TO, {}, ":containerOf",
                    Expand(EXPAND::IN, "Forum",
                      ForeachRship(RSHIP_DIR::FROM, {}, ":hasModerator",
                        Expand(EXPAND::OUT, "Person",
                          Project({{4, "id", FTYPE::UINT64}, {4, "title", FTYPE::STRING}, {6, "id", FTYPE::UINT64},
                               {6, "firstName", FTYPE::STRING}, {6, "lastName", FTYPE::STRING}}, 
                            Collect())))))))));

  arg_builder ab;
  ab.arg(1, "Comment");
  ab.arg(2, messageId);
  ab.arg(3, ":replyOf");
  ab.arg(4, "Post");
  ab.arg(5, ":containerOf");
  ab.arg(6, "Forum");
  ab.arg(7, ":hasModerator");
  ab.arg(8, "Person");

  if(!compiled) {
    auto c_s = std::chrono::steady_clock::now();
    qeng.generate(q, adaptive);
    compiled = true;
    auto c_e = std::chrono::steady_clock::now();
      std::cout << "Compilation: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(c_e-c_s).count()
        << " ms" << std::endl;
  }
/*
  auto e_s = std::chrono::steady_clock::now();
  qeng.run(&rs, ab.args, false);
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();
*/
  auto e_s = std::chrono::steady_clock::now();
  gdb->begin_transaction();
  qeng.run_parallel(&rs, ab, 24);
  gdb->commit_transaction();
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();

  std::cout << "Execution: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count()
        << " ms" << std::endl;
  runtimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count());
}

void ldbc_jit_is_query_7_p(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t messageId) {
  auto sort_fct = [&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) < boost::get<boost::posix_time::ptime>(qr2[2]); };

  auto q1 = Scan("Post",
            Filter(EQ(Key(0, "id"), Int(messageId)),
              ForeachRship(RSHIP_DIR::FROM, {}, ":hasCreator",
                Expand(EXPAND::OUT, "Person", End()))));

  auto q2 = Scan("Post",
            Filter(EQ(Key(0, "id"), Int(messageId)),
              ForeachRship(RSHIP_DIR::TO, {}, ":replyOf",
                Expand(EXPAND::IN, "Comment", 
                  ForeachRship(RSHIP_DIR::FROM, {}, ":hasCreator",
                    Expand(EXPAND::OUT, "Person", 
                      Join(JOIN_OP::LEFT_OUTER, {4, 2}, 
                        Project({
                          {2, "id", FTYPE::UINT64}, {2, "content", FTYPE::STRING}, {2, "creationDate", FTYPE::TIME},
                          {4, "id", FTYPE::UINT64}, {4, "firstName", FTYPE::STRING}, {4, "lastName", FTYPE::STRING}
                          /*{8, "", FTYPE::BOOLEAN}*/
                          }, 
                          /*Sort(sort_fct,*/  Collect()), q1)))))));

  arg_builder ab;
  ab.arg(1, "Post");
  ab.arg(2, messageId);
  ab.arg(3, ":replyOf");
  ab.arg(4, "Comment");
  ab.arg(5, ":hasCreator");
  ab.arg(6, "Person");
  ab.arg(7, "Post");
  ab.arg(8, messageId);
  ab.arg(9, ":hasCreator");
  ab.arg(10, "Person");

  if(!compiled) {
    auto c_s = std::chrono::steady_clock::now();
    qeng.generate(q2, adaptive);
    compiled = true;
    auto c_e = std::chrono::steady_clock::now();
      std::cout << "Compilation: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(c_e-c_s).count()
        << " ms" << std::endl;
  }
/*
  auto e_s = std::chrono::steady_clock::now();
  qeng.run(&rs, ab.args, false);
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();
*/
  auto e_s = std::chrono::steady_clock::now();
  //gdb->begin_transaction();
  qeng.run_parallel(&rs, ab, 24);
  //gdb->commit_transaction();
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();

  std::cout << "Execution: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count()
        << " ms" << std::endl;
  runtimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count());
}

void ldbc_jit_is_query_7_c(graph_db_ptr &gdb, query_engine &qeng, result_set &rs, bool adaptive, uint64_t messageId) {
  auto sort_fct = [&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) < boost::get<boost::posix_time::ptime>(qr2[2]); };
  
  auto q1 = Scan("Comment",
            Filter(EQ(Key(0, "id"), Int(messageId)),
              ForeachRship(RSHIP_DIR::FROM, {}, ":hasCreator",
                Expand(EXPAND::OUT, "Person", End()))));

  auto q2 = Scan("Comment",
            Filter(EQ(Key(0, "id"), Int(messageId)),
              ForeachRship(RSHIP_DIR::TO, {}, ":replyOf",
                Expand(EXPAND::IN, "Comment", 
                  ForeachRship(RSHIP_DIR::FROM, {}, ":hasCreator",
                    Expand(EXPAND::OUT, "Person", 
                      Join(JOIN_OP::LEFT_OUTER, {4, 2}, Project({
                          {2, "id", FTYPE::UINT64}, {2, "content", FTYPE::STRING}, {2, "creationDate", FTYPE::TIME},
                          {4, "id", FTYPE::UINT64}, {4, "firstName", FTYPE::STRING}, {4, "lastName", FTYPE::STRING},
                          {8, "", FTYPE::BOOLEAN}}, 
                        /*Sort(sort_fct, */ Collect()), q1)))))));

  arg_builder ab;
  ab.arg(1, "Comment");
  ab.arg(2, messageId);
  ab.arg(3, ":replyOf");
  ab.arg(4, "Comment");
  ab.arg(5, ":hasCreator");
  ab.arg(6, "Person");
  ab.arg(7, "Comment");
  ab.arg(8, messageId);
  ab.arg(9, ":hasCreator");
  ab.arg(10, "Person");

  if(!compiled) {
    auto c_s = std::chrono::steady_clock::now();
    qeng.generate(q2, adaptive);
    compiled = true;
    auto c_e = std::chrono::steady_clock::now();
      std::cout << "Compilation: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(c_e-c_s).count()
        << " ms" << std::endl;
  }
/*
  auto e_s = std::chrono::steady_clock::now();
  qeng.run(&rs, ab.args, false);
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();
*/
  auto e_s = std::chrono::steady_clock::now();
  //gdb->begin_transaction();
  qeng.run_parallel(&rs, ab, 24);
  //gdb->commit_transaction();
  auto e_e = std::chrono::steady_clock::now();
  rs.data.clear();

  std::cout << "Execution: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count()
        << " ms" << std::endl;
  runtimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(e_e-e_s).count());
}

void run_is_1(graph_db_ptr gdb, query_engine &qeng) {
  std::vector<uint64_t> personIds = {26388279115622, 2199023280088, 19791209323074, 15393162816482, 15393162855246,
        19791209363124, 28587302347895, 10995116305049, 4398046515646, 2199023282192,
        30786325588998, 2199023301366, 19791209333857, 24189255835727, 26388279139007,
        24189255855187, 17592186088378, 6597069782503, 32985348854773, 21990232576546,
        26388279080215, 21990232627526, 15393162855372, 2199023261717, 10995116348358,
        6597069771464, 17592186084059, 15393162854260, 4398046543803, 71623, 32985348891012,
        6597069786049, 30786325626045, 8796093025994, 8796093041362, 13194139568647,
        28587302324409, 10995116329599, 17592186068239, 30786325621884, 13194139605452,
        4398046511596, 13194139597106, 13194139540307, 32985348862047, 28587302352600,
        17592186070889, 8796093026236, 17592186094267, 32985348870938, 21990232600629,
        2199023283094};

  for (auto i = 0u; i < personIds.size(); i++) {
    result_set rs;
    ldbc_jit_is_query_1_a(gdb, qeng, rs, true, personIds[i]);
  }
}

void run_is_2_p(graph_db_ptr gdb, query_engine &qeng) {
  std::vector<uint64_t> personIds = {10995116338469, 21990232591571, 19791209356794, 13194139598683, 68301,
        4398046529914, 21990232626534, 4398046537991, 10995116305964, 30786325620353,
        17592186073576, 58850, 8796093024620, 19791209367505, 2199023275285, 17592186110043,
        10995116287120, 13628, 4398046565656, 6597069772434, 13194139576699, 4398046549865,
        13194139533618, 6597069828622, 15393162790221, 2199023292961, 21990232588012,
        19791209359083, 26388279077904, 15393162809380, 8796093033086, 4398046562129,
        4398046568995, 34565, 6597069822324, 19791209342401, 17592186056186, 15393162814685,
        10995116293981, 15393162815221, 30786325632464, 8796093035558, 26388279073187,
        21990232584928, 4398046512001, 4398046563886, 6597069820569, 13194139565644,
        17592186099787, 10995116282803, 15393162838932, 6597069825699};

  for (auto i = 0u; i < personIds.size(); i++) {
    result_set rs;
    ldbc_jit_is_query_2_p(gdb, qeng, rs, true, personIds[i]);
  }
}

void run_is_2_c(graph_db_ptr gdb, query_engine &qeng) {
  std::vector<uint64_t> personIds = {6597069786683, 4398046519893, 36226, 36675, 8796093036632, 13194139577652,
        10995116308345, 8796093050459, 4398046578984, 4398046530211, 4398046584079,
        10995116294248, 15393162815075, 8796093047284, 17592186050313, 26736,
        28587302371191, 72220, 8599, 10995116301838, 4398046574671, 21990232622930,
        21990232610624, 6597069796889, 26388279075075, 6597069819367, 4398046583349,
        21990232580825, 15393162794693, 13194139540856, 63829, 6597069789305, 21990232556528,
        4398046558107, 2199023273750, 8796093090229, 26388279098001, 15393162805575,
        4398046537118, 19791209361803, 10995116324570, 2199023283682, 10995116322754,
        2199023271643, 4398046562506, 17592186090419, 17592186113138, 2199023298757,
        72607, 8796093052442, 10995116318624, 8796093067457};

  for (auto i = 0u; i < personIds.size(); i++) {
    result_set rs;
    ldbc_jit_is_query_2_c(gdb, qeng, rs, true, personIds[i]);
  }
}

void run_is_3(graph_db_ptr gdb, query_engine &qeng) {
  std::vector<uint64_t> personIds = {2199023269673, 2199023326667, 10995116286165, 4398046514225, 2199023268992,
        17592186092889, 2199023304089, 36915, 15393162822969, 10995116295447,
        2199023272508, 13194139546280, 62739, 10995116303770, 2199023281253,
        15393162792543, 10995116308879, 4398046579349, 13194139594779, 21990232589465,
        38953, 4398046581131, 6597069804810, 4398046535151, 15393162825201, 13194139576334,
        4398046544758, 45360, 16163, 19791209356202, 10995116299448, 2199023317693,
        13194139576572, 15393162802523, 15393162840727, 15393162814789, 18103, 4831,
        30786325627738, 6597069803008, 6597069769310, 13194139597867, 2199023271926,
        2199023281226, 4398046518021, 9598, 13194139585832, 10995116280587, 10995116328660,
        28587302392195, 4398046558180, 42321};

  for (auto i = 0u; i < personIds.size(); i++) {
    result_set rs;
    ldbc_jit_is_query_3(gdb, qeng, rs, true, personIds[i]);
  }
}

void run_is_4_p(graph_db_ptr gdb, query_engine &qeng) {
  std::vector<uint64_t> postIds = { 4398067251028,  2199043946640,  2199037876061,  6597090750692, 4398075704871,
        7696589408397,  3298550269571,  4398061999667,  7696621883870,  7696591812715,
        2748817450668,  5497571035089,  4398055319228, 8246355344067,  8246362564011,
        1649294789710,  7146856770100,  3848300633506,  8246347114898,  5497580138363,
        7146845964022,  7146861667978,  7146850822113,  7146851976510,  7146847600481,
        7146833549023,  7146835916936,  2199031335329,  4947834453085,  1649269718480,
        2748802751725,  2748809026462,  7146849576015,  6597073593366,  6047349141655,
        3848325558344,  5497560701738,  6597099501373,  7146837110089,  5497569013304,
        8246358234322,  549765328557,  5497595205987,  3848298086950,  2199040160941,
        7696586176598,  8246365612544,  3298562064542,  7146853212564, 8246366297574,
        4947814411455,  7146847681997};

  for (auto i = 0u; i < postIds.size(); i++) {
    result_set rs;
    ldbc_jit_is_query_4_p(gdb, qeng, rs, true, postIds[i]);
  }
}

void run_is_4_c(graph_db_ptr gdb, query_engine &qeng) {
  std::vector<uint64_t> commentIds = {7146837074657,  6597103956627,  3848301802195,  7146837499403,  5497571658687,
        7146859489686,  5497570252406,  3848303243210,  5497572320002,  5497579913092,
        2199023675683,  3298567083043,  3298545915339, 6047340077013,  2748810494285,
        3848304697939,  2748793590070,  7146836614405,  8246374073295,  5497575170470,
        7696615613480,  6047342625473, 7146840311129,  5497592913303,  3848302626541,
        549763541252, 7696594987644,  7696616449636,  1649275891208,  8246351688273,
        6597091918954,  7696618491904,  7146831583655,  5497569640572,  6597077682727,
        5497567744687,  4398049179630,  4398073102203,  7146860736359, 4947832362234,
        4398067346974,  7146840849688,  4398083584372,  7146840360913,  3848306595564,
        6047319356807, 8796112244667,  7146849550683,  6047352098252,  7696611407007,
        7146833587239,  5497569176501};

  for (auto i = 0u; i < commentIds.size(); i++) {
    result_set rs;
    ldbc_jit_is_query_4_c(gdb, qeng, rs, true, commentIds[i]);
  }
}

void run_is_5_p(graph_db_ptr gdb, query_engine &qeng) {
  std::vector<uint64_t> postIds = {6047326493568, 3298542751399, 5497579766579, 6047315801577, 7146826860796,
        4398052411492, 6597071125408, 2748795008706, 5497565976239, 6047317955673,
        8246349086875, 6597101144059, 5497585321851, 6047322995696, 8246343732132,
        8246361238663, 8246371250918, 7146841642007, 4398066832413, 2748781695234,
        7146855075933, 7696592838948, 3848319905136, 7146837661310, 7146858026094,
        4398071981665, 7696609373164, 5497573996954, 6597079488539, 3848319718972,
        6597087897092, 7146831804964, 8246377330827, 3848311890261, 3298536481883,
        7696600561652, 5497573589618, 3298556456310, 6597109262960, 549756173665,
        4947834328852, 4398075355527, 7696611725879, 5497566786379, 7696612644083,
        6047332264718, 7146837107713, 7696607396967, 7146857933718, 2748804452443,
        6597099130552, 6597080687795};

  for (auto i = 0u; i < postIds.size(); i++) {
    result_set rs;
    ldbc_jit_is_query_5_p(gdb, qeng, rs, true, postIds[i]);
  }
}

void run_is_5_c(graph_db_ptr gdb, query_engine &qeng) {
  std::vector<uint64_t> commentIds = {6047326493568, 3298542751399, 5497579766579, 6047315801577, 7146826860796,
        4398052411492, 6597071125408, 2748795008706, 5497565976239, 6047317955673,
        8246349086875, 6597101144059, 5497585321851, 6047322995696, 8246343732132,
        8246361238663, 8246371250918, 7146841642007, 4398066832413, 2748781695234,
        7146855075933, 7696592838948, 3848319905136, 7146837661310, 7146858026094,
        4398071981665, 7696609373164, 5497573996954, 6597079488539, 3848319718972,
        6597087897092, 7146831804964, 8246377330827, 3848311890261, 3298536481883,
        7696600561652, 5497573589618, 3298556456310, 6597109262960, 549756173665,
        4947834328852, 4398075355527, 7696611725879, 5497566786379, 7696612644083,
        6047332264718, 7146837107713, 7696607396967, 7146857933718, 2748804452443,
        6597099130552, 6597080687795};

  for (auto i = 0u; i < commentIds.size(); i++) {
    result_set rs;
    ldbc_jit_is_query_5_c(gdb, qeng, rs, true, commentIds[i]);
  }
}

void run_is_6_p(graph_db_ptr gdb, query_engine &qeng) {
  std::vector<uint64_t> postIds = {6047348850997, 5497564365221, 3848329967558, 4398047599412, 6597105769994,
        4947821105898, 7696607613655, 5497597471779, 5497565124879, 7696587911380,
        7696592464558, 6047339895007, 7146837287657, 7696592802214, 6047333509576,
        6047318254791, 3298568940677, 4947820585024, 8246339385108, 3298562096579,
        7696617491509, 5497569775062, 8246366523162, 3298574334235, 4947820310842,
        4398085624920, 7696594106627, 5497583929848, 8246345580501, 8796110519993,
        1649283295282, 2199051023689, 6047318362421, 6047314584628, 7696589261990,
        6047344893312, 3298550621323, 6597092944738, 6047321024153, 4398054812559,
        4398071972522, 3848293901086, 8246355392461, 2748800645630, 1649267888051,
        7146857571848, 6597093766004, 4947837595380, 8246344422919, 7696611410177,
        5497583687275, 2748794674509};

  for (auto i = 0u; i < postIds.size(); i++) {
    result_set rs;
    ldbc_jit_is_query_6_p(gdb, qeng, rs, true, postIds[i]);
  }
}

void run_is_6_c(graph_db_ptr gdb, query_engine &qeng) {
  std::vector<uint64_t> commentIds = {7146848797209, 6047342252109, 8246377297601, 1099513403506, 1649297650520,
        8246341221876, 8246367373329, 8246342269850, 8246340778158, 6047344491668,
        8796115187591, 7696619489960, 1099521487215, 6047326425751, 8246351033697,
        8246365196867, 8246343099694, 6047350744524, 5497593227720, 2748790628283,
        2748802395151, 3298557085552, 7696621423568, 7696613855335, 8246359053718,
        7146844090065, 8246352454414, 5497577976061, 4947829453022, 6597092286565,
        7146831749672, 6597075028453, 8246340918420, 3848318579211, 6597085643326,
        3298542288861, 7696613107298, 6597087895623, 6597088834693, 8246358246770,
        7696610350084, 7696589652492, 3298570627344, 8246352772973, 8246347750453,
        8246342872094, 6597093391156, 7146827520774, 4947816404504, 7146854504754,
        8246373280377, 8246367726351};

  for (auto i = 0u; i < commentIds.size(); i++) {
    result_set rs;
    ldbc_jit_is_query_6_c(gdb, qeng, rs, true, commentIds[i]);
  }
}

void run_is_7_p(graph_db_ptr gdb, query_engine &qeng) {
  std::vector<uint64_t> postIds = {4398074288624, 3298546507517, 5497587033299, 4398052366789, 6047338065986,
        8246359029970, 7146840417476, 6597099197111, 6047334688191, 4947831710060,
        2748817142166, 1649296572103, 5497575564527, 2199062360690, 6047335741465,
        2199044209868, 4398061553596, 7146844362356, 4947825543184, 6047344045696,
        8246348974651, 2199034832840, 6597082193070, 8246348170051, 6597107012577,
        6597101836499, 6047346170037, 4398081795065, 3298564597702, 8246368178974,
        2748818954854, 6047342805070, 5497562418044, 5497590305765, 5497590128285,
        4398086919921, 3848319343421, 8246361188746, 17215163, 7696621510202,
        8246341795356, 1099533817307, 4398082691758, 5497589393851, 7146828897760,
        5497570220329, 7696596013117, 1099514518363, 6047316070076, 7146847131440,
        5497559818606, 4398078795093};

  for (auto i = 0u; i < postIds.size(); i++) {
    result_set rs;
    ldbc_jit_is_query_7_p(gdb, qeng, rs, true, postIds[i]);
  }
}

void run_is_7_c(graph_db_ptr gdb, query_engine &qeng) {
  std::vector<uint64_t> commentIds = {7146846240480, 5497578410380, 5497572128778, 3298557786474, 3298564869730,
        6597085241772, 5497562407626, 7146865899861, 2199063215958, 3298543129796,
        7696585937397, 3848303485145, 7696584725469, 8246372882931, 4624321,
        7696598517932, 6597097715505, 5497574002790, 2748783314629, 4947806953871,
        2199027045720, 8246351169549, 2199045162022, 4398062238469, 3298555317270,
        1649290760770, 3848327767348, 8246366635007, 8246346947901, 1099532289463,
        3848300985206, 5497581299875, 5497579358934, 2199063595159, 7696607737987,
        6047353121515, 7696590312905, 5497593006914, 6597089610781, 7146832026578,
        7696595067279, 1099545270694, 5497564138318, 1099537597905, 6047325524027,
        3298545496742, 5497572456680, 3848302106259, 2748794743559, 7146846003719,
        8246346437108, 7696607465071};

  for (auto i = 0u; i < commentIds.size(); i++) {
    result_set rs;
    ldbc_jit_is_query_7_c(gdb, qeng, rs, true, commentIds[i]);
  }
}

void run_benchmark(graph_db_ptr gdb, query_engine &qeng) {
  /*run_is_1(gdb, qeng);  
  spdlog::info("Query #1: {} msecs", calc_avg());
  qeng.cleanup();

  run_is_2_p(gdb, qeng);  
  spdlog::info("Query #2: {} msecs", calc_avg());
  qeng.cleanup();

  run_is_2_c(gdb, qeng);  
  spdlog::info("Query #3: {} msecs", calc_avg());
  qeng.cleanup();

  run_is_3(gdb, qeng);  
  spdlog::info("Query #4: {} msecs", calc_avg());
  qeng.cleanup();

  run_is_4_p(gdb, qeng);  
  spdlog::info("Query #5: {} msecs", calc_avg());
  qeng.cleanup();

  run_is_4_c(gdb, qeng);  
  spdlog::info("Query #6: {} msecs", calc_avg());
  qeng.cleanup();

  run_is_5_p(gdb, qeng);  
  spdlog::info("Query #7: {} msecs", calc_avg());
  qeng.cleanup();

  run_is_5_c(gdb, qeng);  
  spdlog::info("Query #8: {} msecs", calc_avg());
  qeng.cleanup();

  run_is_6_p(gdb, qeng);  
  spdlog::info("Query #9: {} msecs", calc_avg());
  qeng.cleanup();

  run_is_6_c(gdb, qeng);  
  spdlog::info("Query #10: {} msecs", calc_avg());
  qeng.cleanup();*/

  run_is_7_p(gdb, qeng);  
  spdlog::info("Query #11: {} msecs", calc_avg());
  qeng.cleanup();

  run_is_7_c(gdb, qeng);  
  spdlog::info("Query #12: {} msecs", calc_avg());
/*
  ldbc_jit_is_query_2_p(gdb, qeng, rs, false, 10995116338469);
  //std::cout << rs << std::endl;
  spdlog::info("Query #2: {} msecs", 5);
  rs.data.clear();

  ldbc_jit_is_query_2_c(gdb, qeng, rs, false, 8796093050459);
  //std::cout << rs << std::endl;
  spdlog::info("Query #3: {} msecs", 5);
  rs.data.clear();

  ldbc_jit_is_query_3(gdb, qeng, rs, false, 13194139546280);
  //std::cout << rs << std::endl;
  spdlog::info("Query #4: {} msecs", 5);
  rs.data.clear();

  ldbc_jit_is_query_4_p(gdb, qeng, rs, false, 4398055319228);
  //std::cout << rs << std::endl;
  spdlog::info("Query #5: {} msecs", 5);
  rs.data.clear();

  ldbc_jit_is_query_4_c(gdb, qeng, rs, false, 3298545915339);
  //std::cout << rs << std::endl;
  spdlog::info("Query #6: {} msecs", 5);
  rs.data.clear();

  ldbc_jit_is_query_5_p(gdb, qeng, rs, false, 5497585321851);
  //std::cout << rs << std::endl;
  spdlog::info("Query #7: {} msecs", 5);
  rs.data.clear();

  ldbc_jit_is_query_5_c(gdb, qeng, rs, false, 7146830729841);
  //std::cout << rs << std::endl;
  spdlog::info("Query #8: {} msecs", 5);
  rs.data.clear();

  ldbc_jit_is_query_6_p(gdb, qeng, rs, false, 8246366523162);
  //std::cout << rs << std::endl;
  spdlog::info("Query #9: {} msecs", 5);
  rs.data.clear();

  ldbc_jit_is_query_6_c(gdb, qeng, rs, false, 6047350744524);
  //std::cout << rs << std::endl;
  spdlog::info("Query #10: {} msecs", 5);
  rs.data.clear();

  ldbc_jit_is_query_7_p(gdb, qeng, rs, false, 4398061553596);
  std::cout << rs << std::endl;
  spdlog::info("Query #11: {} msecs", 5);
  rs.data.clear();

  ldbc_jit_is_query_7_c(gdb, qeng, rs, false, 5497574002790);
  std::cout << rs << std::endl;
  spdlog::info("Query #12: {} msecs", 5);*/
}

int main(int argc, char **argv) {
  bool strict = false;
  std::string pool_path, db_name;
  std::string snb_home =
#ifdef SF_10
    "/home/data/SNB_SF_10/";
#else
    "/home/data/SNB_SF_1/";
#endif

 try {
    options_description desc{"Options"};
    desc.add_options()
        ("help,h", "Help")
        ("verbose,v", bool_switch()->default_value(false), "Verbose - show debug output")
        ("strict,s", bool_switch()->default_value(false), "Strict mode - assumes that all columns contain values of the same type")
        ("pool,p", value<std::string>(&pool_path)->required(), "Path to the PMem pool")
        ("import,i", value<std::string>(&snb_home), "Path to directories containing SNB CSV files")
        ("db,d", value<std::string>(&db_name)->required(),"Database name (required)");

    variables_map vm;
    store(parse_command_line(argc, argv, desc), vm);

    if (vm.count("help")) {
      std::cout << "Poseidon Graph Database LDBC Benchmark Version " << POSEIDON_VERSION
                << "\n"
                << desc << '\n';
      return -1;
    }
    if (vm.count("import"))
      snb_home = vm["import"].as<std::string>();

    if (vm.count("strict"))
      strict = vm["strict"].as<bool>();

    if (vm.count("db_name"))
      db_name = vm["db_name"].as<std::string>();

    if (vm.count("pool"))
      pool_path = vm["pool"].as<std::string>();

    notify(vm);

      } catch (const error &ex) {
    std::cerr << ex.what() << '\n';
    return -1;
  }

#ifdef USE_PMDK
    auto pool = graph_pool::open(pool_path);
    auto graph = pool->open_graph(db_name);
    #ifdef FPTree
    fptree_recovery(graph);
    #endif
#else
  auto pool = graph_pool::create(pool_path);
  auto graph = pool->create_graph(db_name);

  load_snb_data(graph, snb_home);
#endif
  graph->print_stats();
	auto THREAD_NUM = 4;
	auto chunks = graph->get_nodes()->num_chunks();
	auto cv_range = chunks / THREAD_NUM;

	query_engine queryEngine(graph, THREAD_NUM, cv_range);

  for (auto i = 0; i < 1; i++) {
    std::cout << "Run: " << i << "\n";
    run_benchmark(graph, queryEngine);
  }
}
