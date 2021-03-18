#include <iostream>
#include <boost/variant.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "defs.hpp"
#include "graph_db.hpp"
#include "graph_pool.hpp"
#include "ldbc.hpp"
#include "config.h"

#include <cassert>
#include "threadsafe_queue.hpp"

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

#define IU_RESULT

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_iu_query_1(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Place", "id", boost::get<uint64_t>(params[0]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Place")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Place", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif
#endif

  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Tag", "id", boost::get<uint64_t>(params[1]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Tag")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#else
                .nodes_where("Tag", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#endif
#endif

  auto q3 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Organisation", "id", boost::get<uint64_t>(params[2]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Organisation")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#else
                .nodes_where("Organisation", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#endif
#endif

  auto q4 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Organisation", "id", boost::get<uint64_t>(params[3]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Organisation")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
#else
                .nodes_where("Organisation", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
#endif
#endif

  auto q5 = query(gdb).create("Person",
                              {{"id", boost::any(boost::get<uint64_t>(params[4]))},
                              {"firstName", boost::any(boost::get<std::string &>(params[5]))},
                              {"lastName", boost::any(boost::get<std::string &>(params[6]))},
                              {"gender", boost::any(boost::get<std::string &>(params[7]))},
                              {"birthday", boost::any(boost::get<std::string &>(params[8]))},
                              {"creationDate", boost::any(boost::get<std::string &>(params[9]))},
                              {"locationIP", boost::any(boost::get<std::string &>(params[10]))},
                              {"browserUsed", boost::any(boost::get<std::string &>(params[11]))},
                              {"language", boost::any(boost::get<std::string &>(params[12]))},
                              {"email", boost::any(boost::get<std::string &>(params[13]))}})
                      .crossjoin(q1)
                      .create_rship({0, 1}, ":isLocatedIn", {})
                      .crossjoin(q2)
                      .create_rship({0, 3}, ":hasInterest", {})
                      .crossjoin(q3)
                      .create_rship({0, 5}, ":studyAt", {{"classYear", boost::any(boost::get<int>(params[14]))}})
                      .crossjoin(q4)
                      .create_rship({0, 7}, ":workAt", {{"workFrom", boost::any(boost::get<int>(params[15]))}})
#ifdef IU_RESULT
                      .collect(rs);
#else
                      .finish();
#endif

  query::start({&q1, &q2, &q3, &q4, &q5});
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_iu_query_2(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Post", "id", boost::get<uint64_t>(params[0]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif
#endif

  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", boost::get<uint64_t>(params[1]))
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#else
                .nodes_where("Person", "id",
                                  [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#endif
#endif
                .crossjoin(q1)
                .create_rship({0, 1}, ":likes", {{"creationDate", boost::any(boost::get<std::string &>(params[2]))}})
#ifdef IU_RESULT
                .collect(rs);
#else
                .finish();
#endif

  query::start({&q1, &q2});
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_iu_query_3(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Comment", "id", boost::get<uint64_t>(params[0]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Comment")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif
#endif

  auto q2 =
      query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", boost::get<uint64_t>(params[1]))
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#else
          .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#endif
#endif
          .crossjoin(q1)
          .create_rship({0, 1}, ":likes", {{"creationDate", boost::any(boost::get<std::string &>(params[2]))}})
#ifdef IU_RESULT
          .collect(rs);
#else
          .finish();
#endif

  query::start({&q1, &q2});
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_iu_query_4(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", boost::get<uint64_t>(params[0]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif
#endif

  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Tag", "id", boost::get<uint64_t>(params[1]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Tag")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#else
                .nodes_where("Tag", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#endif
#endif

  auto q3 = query(gdb).create("Forum",
                              {{"id", boost::any(boost::get<uint64_t>(params[2]))},
                              {"title", boost::any(boost::get<std::string &>(params[3]))},
                              {"creationDate", boost::any(boost::get<std::string &>(params[4]))} })
                      .crossjoin(q1)
                      .create_rship({0, 1}, ":hasModerator", {})
                      .crossjoin(q2)
                      .create_rship({0, 3}, ":hasTag", {})
#ifdef IU_RESULT
                      .collect(rs);
#else
                      .finish();
#endif

  query::start({&q1, &q2, &q3});
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_iu_query_5(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q1 =
      query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", boost::get<uint64_t>(params[0]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
          .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif
#endif

  auto q2 =
      query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Forum", "id", boost::get<uint64_t>(params[1]))
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Forum")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#else
          .nodes_where("Forum", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#endif
#endif
          .crossjoin(q1)
          .create_rship({0, 1}, ":hasMember", {{"creationDate", boost::any(boost::get<std::string &>(params[2]))}})
#ifdef IU_RESULT
          .collect(rs);
#else
          .finish();
#endif

  query::start({&q1, &q2});
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_iu_query_6(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", boost::get<uint64_t>(params[0]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif
#endif

  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Forum", "id", boost::get<uint64_t>(params[1]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Forum")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#else
                .nodes_where("Forum", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#endif
#endif

  auto q3 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Place", "id", boost::get<uint64_t>(params[2]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Place")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#else
                .nodes_where("Place", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#endif
#endif

  auto q4 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Tag", "id", boost::get<uint64_t>(params[3]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Tag")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
#else
                .nodes_where("Tag", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
#endif
#endif

  auto q5 = query(gdb).create("Post",
                            {{"id", boost::any(boost::get<uint64_t>(params[4]))},
                              {"imageFile", boost::any(boost::get<std::string &>(params[5]))},
                              {"creationDate", boost::any(boost::get<std::string &>(params[6]))},
                              {"locationIP", boost::any(boost::get<std::string &>(params[7]))},
                              {"browserUsed", boost::any(boost::get<std::string &>(params[8]))},
                              {"language", boost::any(boost::get<std::string &>(params[9]))},
                              {"content", boost::any(boost::get<std::string &>(params[10]))},
                              {"length", boost::any(boost::get<int>(params[11]))} })
                      .crossjoin(q1)
                      .create_rship({0, 1}, ":hasCreator", {})
                      .crossjoin(q2)
                      .create_rship({3, 0}, ":containerOf", {})
                      .crossjoin(q3)
                      .create_rship({0, 5}, ":isLocatedIn", {})
                      .crossjoin(q4)
                      .create_rship({0, 7}, ":hasTag", {})
#ifdef IU_RESULT
                      .collect(rs);
#else
                      .finish();
#endif

  query::start({&q1, &q2, &q3, &q4, &q5});
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_iu_query_7(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q1 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", boost::get<uint64_t>(params[0]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif
#endif

  auto q2 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Post", "id", boost::get<uint64_t>(params[1]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#endif
#endif

  auto q3 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Place", "id", boost::get<uint64_t>(params[2]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Place")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#else
                .nodes_where("Place", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#endif
#endif

  auto q4 = query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Tag", "id", boost::get<uint64_t>(params[3]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Tag")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
#else
                .nodes_where("Tag", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
#endif
#endif

  auto q5 = query(gdb).create("Comment",
                              {{"id", boost::any(boost::get<uint64_t>(params[4]))},
                              {"creationDate", boost::any(boost::get<std::string &>(params[5]))},
                              {"locationIP", boost::any(boost::get<std::string &>(params[6]))},
                              {"browserUsed", boost::any(boost::get<std::string &>(params[7]))},
                              {"content", boost::any(boost::get<std::string &>(params[8]))},
                              {"length", boost::any(boost::get<int>(params[9]))} })
                      .crossjoin(q1)
                      .create_rship({0, 1}, ":hasCreator", {})
                      .crossjoin(q2)
                      .create_rship({0, 3}, ":replyOf", {})
                      .crossjoin(q3)
                      .create_rship({0, 5}, ":isLocatedIn", {})
                      .crossjoin(q4)
                      .create_rship({0, 7}, ":hasTag", {})
#ifdef IU_RESULT
                      .collect(rs);
#else
                      .finish();
#endif

  query::start({&q1, &q2, &q3, &q4, &q5});
}

// ------------------------------------------------------------------------------------------------------------------------

void ldbc_iu_query_8(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

  auto q1 =
      query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", boost::get<uint64_t>(params[0]));
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
          .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif
#endif

  auto q2 =
      query(gdb)
#ifdef RUN_INDEXED
               .nodes_where_indexed("Person", "id", boost::get<uint64_t>(params[1]))
#else
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#else
          .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#endif
#endif
          .crossjoin(q1)
          .create_rship({0, 1}, ":knows", {{"creationDate", boost::any(boost::get<std::string &>(params[2]))}})
#ifdef IU_RESULT
          .collect(rs);
#else
          .finish();
#endif

  query::start({&q1, &q2});
}