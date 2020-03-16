#include "ldbc.hpp"
#include "qop.hpp"
#include "query.hpp"

#define RUN_PARALLEL

namespace pj = builtin;

void ldbc_is_query_1(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
  
  auto q = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(personId); })
#else
               .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
#endif
               .from_relationships(":isLocatedIn")
               .to_node("Place")
               .project({PExpr_(0, pj::string_property(res, "firstName")),
                         PExpr_(0, pj::string_property(res, "lastName")),
                         PExpr_(0, pj::pr_date(res, "birthday")),
                         PExpr_(0, pj::string_property(res, "locationIP")),
                         PExpr_(0, pj::string_property(res, "browserUsed")),
                         PExpr_(2, pj::uint64_property(res, "id")),
                         PExpr_(0, pj::string_property(res, "gender")),
                         PExpr_(0, pj::ptime_property(res, "creationDate")) })
               .collect(rs);
  q.start();
  rs.wait();
}

void ldbc_is_query_2_p(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
  auto maxHops = 100; 

  auto q = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(personId); })
#else
               .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
#endif
               .to_relationships(":hasCreator")
               .from_node("Post")
               .project({PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(2, !pj::string_property(res, "content").empty() ? 
                            pj::string_property(res, "content") : pj::string_property(res, "imageFile")),
                        PExpr_(2, pj::ptime_property(res, "creationDate")),
                        PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::string_property(res, "firstName")),
                        PExpr_(0, pj::string_property(res, "lastName")) })
               .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) > boost::get<boost::posix_time::ptime>(qr2[2]); })
               .limit(10)
               .collect(rs);

  q.start();
  rs.wait();
}

void ldbc_is_query_2_c(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
  auto maxHops = 100; 
  
  auto q = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(personId); })
#else
               .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
#endif
               .to_relationships(":hasCreator")
               .from_node("Comment")
               .from_relationships({1, maxHops}, ":replyOf") 
               .to_node("Post")
               .from_relationships(":hasCreator")
               .to_node("Person")
               .project({PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(2, pj::string_property(res, "content")),
                        PExpr_(2, pj::ptime_property(res, "creationDate")),
                        PExpr_(4, pj::uint64_property(res, "id")),
                        PExpr_(6, pj::uint64_property(res, "id")),
                        PExpr_(6, pj::string_property(res, "firstName")),
                        PExpr_(6, pj::string_property(res, "lastName")) })
               .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) > boost::get<boost::posix_time::ptime>(qr2[2]); })
               .limit(10)
               .collect(rs);

  q.start();
  rs.wait();
}

void ldbc_is_query_3(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {

  auto q = query(gdb)
#ifdef RUN_PARALLEL
                .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(personId); })
#else
                .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(personId); })
#endif
                .from_relationships(":knows")
                .to_node("Person")
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "firstName")),
                          PExpr_(2, pj::string_property(res, "lastName")),
                          PExpr_(1, pj::ptime_property(res, "creationDate"))})
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                          if (boost::get<boost::posix_time::ptime>(qr1[3]) == boost::get<boost::posix_time::ptime>(qr2[3]))
                            return boost::get<uint64_t>(qr1[0]) < boost::get<uint64_t>(qr2[0]);
                          return boost::get<boost::posix_time::ptime>(qr1[3]) > boost::get<boost::posix_time::ptime>(qr2[3]); })
                .collect(rs);
  
  q.start();
  rs.wait();
}

void ldbc_is_query_4_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {

	auto q = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .project({PExpr_(0, pj::ptime_property(res, "creationDate")),
                          PExpr_(0, !pj::string_property(res, "content").empty() ? 
                            pj::string_property(res, "content") : pj::string_property(res, "imageFile")) })
                .collect(rs);
				
	q.start();
	rs.wait();
}

void ldbc_is_query_4_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {

  auto q = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Comment")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .project({PExpr_(0, pj::ptime_property(res, "creationDate")),
                          PExpr_(0, pj::string_property(res, "content")) })
                .collect(rs);
				
	q.start();
	rs.wait();
}

void ldbc_is_query_5_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {

	auto q = query(gdb)
#ifdef RUN_PARALLEL
                .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .from_relationships(":hasCreator")
                .to_node("Person")
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "firstName")),
                          PExpr_(2, pj::string_property(res, "lastName")) })
                .collect(rs);

	q.start();
	rs.wait();
}

void ldbc_is_query_5_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {

  auto q = query(gdb)
#ifdef RUN_PARALLEL
                .all_nodes()
               .has_label("Comment")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .from_relationships(":hasCreator")
                .to_node("Person")
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "firstName")),
                          PExpr_(2, pj::string_property(res, "lastName")) })
                .collect(rs);

	q.start();
	rs.wait();
}

void ldbc_is_query_6_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
  auto maxHops = 100;
    
  auto q = query(gdb)
#ifdef RUN_PARALLEL
                .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .to_relationships(":containerOf")
                .from_node("Forum")
                .from_relationships(":hasModerator")
                .to_node("Person")
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "title")),
                          PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "firstName")),
                          PExpr_(4, pj::string_property(res, "lastName")) })
                .collect(rs);
	
  q.start();
	rs.wait(); 
}

void ldbc_is_query_6_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
  auto maxHops = 100;
  
  auto q = query(gdb)
#ifdef RUN_PARALLEL
                .all_nodes()
               .has_label("Comment")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .from_relationships({1, maxHops}, ":replyOf") 
                .to_node("Post")
                .to_relationships(":containerOf")
                .from_node("Forum")
                .from_relationships(":hasModerator")
                .to_node("Person")
                .project({PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "title")),
                          PExpr_(6, pj::uint64_property(res, "id")),
                          PExpr_(6, pj::string_property(res, "firstName")),
                          PExpr_(6, pj::string_property(res, "lastName")) })
                .collect(rs);
	
  q.start();
	rs.wait(); 
}

void ldbc_is_query_7_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
     
  auto q1 = query(gdb)
#ifdef RUN_PARALLEL
                .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .from_relationships(":hasCreator")
                .to_node("Person");
  
  auto q2 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .to_relationships(":replyOf")    
                .from_node("Comment")
                .from_relationships(":hasCreator")
                .to_node("Person")
                .outerjoin({4, 2}, q1)
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "content")),
                          PExpr_(2, pj::ptime_property(res, "creationDate")),
                          PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "firstName")),
                          PExpr_(4, pj::string_property(res, "lastName")),
                          PExpr_(8, pj::string_rep(res) == "[0]{}" ?
                                      std::string("false") : std::string("true")) })
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) < boost::get<boost::posix_time::ptime>(qr2[2]); })
                .collect(rs);

  query::start({&q1, &q2});
	rs.wait();
}

void ldbc_is_query_7_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
     
  auto q1 = query(gdb)
#ifdef RUN_PARALLEL
                .all_nodes()
               .has_label("Comment")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .from_relationships(":hasCreator")
                .to_node("Person");
  
  auto q2 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Comment")
               .property( "id",
                           [&](auto &p) { return p.equal(messageId); })
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(messageId); })
#endif
                .to_relationships(":replyOf")    
                .from_node("Comment")
                .from_relationships(":hasCreator")
                .to_node("Person")
                .outerjoin({4, 2}, q1)
                .project({PExpr_(2, pj::uint64_property(res, "id")),
                          PExpr_(2, pj::string_property(res, "content")),
                          PExpr_(2, pj::ptime_property(res, "creationDate")),
                          PExpr_(4, pj::uint64_property(res, "id")),
                          PExpr_(4, pj::string_property(res, "firstName")),
                          PExpr_(4, pj::string_property(res, "lastName")),
                          PExpr_(8, pj::string_rep(res) == "[0]{}" ?
                                      std::string("false") : std::string("true")) })
                .orderby([&](const qr_tuple &qr1, const qr_tuple &qr2) {
                        if (boost::get<boost::posix_time::ptime>(qr1[2]) == boost::get<boost::posix_time::ptime>(qr2[2]))
                          return boost::get<uint64_t>(qr1[0]) > boost::get<uint64_t>(qr2[0]);
                        return boost::get<boost::posix_time::ptime>(qr1[2]) < boost::get<boost::posix_time::ptime>(qr2[2]); })
                .collect(rs);

  query::start({&q1, &q2});
	rs.wait();
}

void ldbc_iu_query_1(graph_db_ptr &gdb, result_set &rs, params_tuple &params) { 
  
  auto q1 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Place")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Place", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif

  auto q2 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Tag")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#else
                .nodes_where("Tag", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#endif

  auto q3 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Organisation")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#else
                .nodes_where("Organisation", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#endif

  auto q4 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Organisation")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
#else
                .nodes_where("Organisation", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
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
                      .collect(rs);

  query::start({&q1, &q2, &q3, &q4, &q5});
}

void ldbc_iu_query_2(graph_db_ptr &gdb, result_set &rs, params_tuple &params) { 

  auto q1 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif

  auto q2 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#else
                .nodes_where("Person", "id",
                                  [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#endif
                .crossjoin(q1)
                .create_rship({0, 1}, ":likes", {{"creationDate", boost::any(boost::get<std::string &>(params[2]))}})
                .collect(rs);

  query::start({&q1, &q2});
}

void ldbc_iu_query_3(graph_db_ptr &gdb, result_set &rs, params_tuple &params) { 

  auto q1 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif

  auto q2 =
      query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#else
          .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#endif
          .crossjoin(q1)
          .create_rship({0, 1}, ":likes", {{"creationDate", boost::any(boost::get<std::string &>(params[2]))}})
          .collect(rs);

  query::start({&q1, &q2});
}

void ldbc_iu_query_4(graph_db_ptr &gdb, result_set &rs, params_tuple &params) { 

  auto q1 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif

  auto q2 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Tag")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#else
                .nodes_where("Tag", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#endif

  auto q3 = query(gdb).create("Forum",
                              {{"id", boost::any(boost::get<uint64_t>(params[2]))},
                              {"title", boost::any(boost::get<std::string &>(params[3]))},
                              {"creationDate", boost::any(boost::get<std::string &>(params[4]))} })
                      .crossjoin(q1)
                      .create_rship({0, 1}, ":hasModerator", {})
                      .crossjoin(q2)
                      .create_rship({0, 3}, ":hasTag", {})
                      .collect(rs);

  query::start({&q1, &q2, &q3});
}

void ldbc_iu_query_5(graph_db_ptr &gdb, result_set &rs, params_tuple &params) { 

  auto q1 = 
      query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
          .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif

  auto q2 =
      query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Forum")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#else
          .nodes_where("Forum", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#endif
          .crossjoin(q1)
          .create_rship({0, 1}, ":hasMember", {{"creationDate", boost::any(boost::get<std::string &>(params[2]))}})
          .collect(rs);

  query::start({&q1, &q2});
}

void ldbc_iu_query_6(graph_db_ptr &gdb, result_set &rs, params_tuple &params) { 

  auto q1 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif

  auto q2 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Forum")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#else
                .nodes_where("Forum", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#endif

  auto q3 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Place")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#else
                .nodes_where("Place", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#endif

  auto q4 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Tag")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
#else
                .nodes_where("Tag", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
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
                      .collect(rs);

  query::start({&q1, &q2, &q3, &q4, &q5});
}

void ldbc_iu_query_7(graph_db_ptr &gdb, result_set &rs, params_tuple &params) { 

  auto q1 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
                .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif

  auto q2 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Post")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#else
                .nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); });
#endif

  auto q3 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Place")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#else
                .nodes_where("Place", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[2])); });
#endif

  auto q4 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Tag")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
#else
                .nodes_where("Tag", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[3])); });
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
                      .collect(rs);

  query::start({&q1, &q2, &q3, &q4, &q5});
}

void ldbc_iu_query_8(graph_db_ptr &gdb, result_set &rs, params_tuple &params) { 

  auto q1 = 
      query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#else
          .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[0])); });
#endif

  auto q2 =
      query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Person")
               .property( "id",
                           [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#else
          .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(boost::get<uint64_t>(params[1])); })
#endif
          .crossjoin(q1)
          .create_rship({0, 1}, ":knows", {{"creationDate", boost::any(boost::get<std::string &>(params[2]))}})
          .collect(rs);

  query::start({&q1, &q2});
}
