#include "ldbc.hpp"
#include "qop.hpp"
#include "query.hpp"

#define RUN_PARALLEL

namespace pj = builtin;

//void ldbc_is_query_1(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
void ldbc_is_query_1(graph_db_ptr &gdb, uint64_t personId) {
  
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
                .print();
                //.collect(rs);
  q.start();
  //rs.wait();
}

//void ldbc_is_query_2_p(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
void ldbc_is_query_2_p(graph_db_ptr &gdb, uint64_t personId) {
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
               .print();
               //.collect(rs);

  //query::start({&q1, &q2});
  q.start();
  //rs.wait();
}

//void ldbc_is_query_2_c(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
void ldbc_is_query_2_c(graph_db_ptr &gdb, uint64_t personId) {
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
               .print();
               //.collect(rs);

  //query::start({&q1, &q2});
  q.start();
  //rs.wait();
}

//void ldbc_is_query_3(graph_db_ptr &gdb, result_set &rs, uint64_t personId) {
void ldbc_is_query_3(graph_db_ptr &gdb, uint64_t personId) {

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
                .print();
                //.collect(rs);
  
  q.start();
  //rs.wait();
}

//void ldbc_is_query_4_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
void ldbc_is_query_4_p(graph_db_ptr &gdb, uint64_t messageId) {

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
                .print();
                //.collect(rs);
				
	q.start();
	//rs.wait();
}

//void ldbc_is_query_4_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
void ldbc_is_query_4_c(graph_db_ptr &gdb, uint64_t messageId) {

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
                .print();
                //.collect(rs);
				
	q.start();
	//rs.wait();
}

//void ldbc_is_query_5_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
void ldbc_is_query_5_p(graph_db_ptr &gdb, uint64_t messageId) {

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
                .print();
                //.collect(rs);

	q.start();
	//rs.wait();
}

//void ldbc_is_query_5_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
void ldbc_is_query_5_c(graph_db_ptr &gdb, uint64_t messageId) {

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
                .print();
                //.collect(rs);

	q.start();
	//rs.wait();
}

//void ldbc_is_query_6_p(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
void ldbc_is_query_6_p(graph_db_ptr &gdb, uint64_t messageId) {
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
                .print();
                //.collect(rs);
	
  q.start();
	//rs.wait(); 
}

//void ldbc_is_query_6_c(graph_db_ptr &gdb, result_set &rs, uint64_t messageId) {
void ldbc_is_query_6_c(graph_db_ptr &gdb, uint64_t messageId) {
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
                .print();
                //.collect(rs);
	
  q.start();
	//rs.wait(); 
}

//void ldbc_is_query_7(graph_db_ptr &gdb, result_set &rs, uint64_t commentId) {
void ldbc_is_query_7(graph_db_ptr &gdb, uint64_t commentId) {
     
  auto q1 = query(gdb)
#ifdef RUN_PARALLEL
                .all_nodes()
               .has_label("Comment")
               .property( "id",
                           [&](auto &p) { return p.equal(commentId); })
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(commentId); })
#endif
                .from_relationships(":hasCreator")
                .to_node("Person");
  
  auto q2 = query(gdb)
#ifdef RUN_PARALLEL
               .all_nodes()
               .has_label("Comment")
               .property( "id",
                           [&](auto &p) { return p.equal(commentId); })
#else
                .nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(commentId); })
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
                .print();
                //.collect(rs);

  query::start({&q1, &q2});
	//rs.wait();
}

//void ldbc_iu_query_1(graph_db_ptr &gdb, result_set &rs) {
void ldbc_iu_query_1(graph_db_ptr &gdb, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props, std::vector<int>& int_props) {
  /*uint64_t personId = 9999999999999;
  auto fName = std::string("New");
  auto lName = std::string("Person");
  auto gender = std::string("female"); 
  auto birthday = std::string("1981-01-21");
  auto creationDate = std::string("2011-01-11T01:51:21.746+0000");
  auto locationIP = std::string("1.183.127.173"); 
  auto browser = std::string("Safari");
  auto language = std::string("\"zh\", \"en\""); 
  auto email = std::string("\"new1@email1.com\", \"new@email2.com\"");
  uint64_t cityId = 505;
  uint64_t tagId = 61;
  uint64_t uniId = 2213;
  uint64_t companyId = 915;
  auto classYear = 2001;
  auto workFrom = 2001;*/

  auto q1 = query(gdb).nodes_where("Place", "id",
                            [&](auto &p) { return p.equal(uint64_props[1]); });

  auto q2 = query(gdb).nodes_where("Tag", "id",
                            [&](auto &p) { return p.equal(uint64_props[2]); });

  auto q3 = query(gdb).nodes_where("Organisation", "id",
                            [&](auto &p) { return p.equal(uint64_props[3]); });

  auto q4 = query(gdb).nodes_where("Organisation", "id",
                            [&](auto &p) { return p.equal(uint64_props[4]); });

  auto q5 = query(gdb).create("Person",
                              {{"id", boost::any(uint64_props[0])},
                              {"firstName", boost::any(str_props[0])},
                              {"lastName", boost::any(str_props[1])},
                              {"gender", boost::any(str_props[2])},
                              {"birthday", boost::any(str_props[3])},
                              {"creationDate", boost::any(str_props[4])},
                              {"locationIP", boost::any(str_props[5])},
                              {"browserUsed", boost::any(str_props[6])},
                              {"language", boost::any(str_props[7])},
                              {"email", boost::any(str_props[8])}})
                      .crossjoin(q1)
                      .create_rship({0, 1}, ":isLocatedIn", {})
                      .crossjoin(q2)
                      .create_rship({0, 3}, ":hasInterest", {})
                      .crossjoin(q3)
                      .create_rship({0, 5}, ":studyAt", {{"classYear", boost::any(int_props[0])}})
                      .crossjoin(q4)
                      .create_rship({0, 7}, ":workAt", {{"workFrom", boost::any(int_props[1])}});
                      //.collect(rs);

  query::start({&q1, &q2, &q3, &q4, &q5});
}

//void ldbc_iu_query_2(graph_db_ptr &gdb, result_set &rs) {
void ldbc_iu_query_2(graph_db_ptr &gdb, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props, std::vector<int>& int_props) {
  
  /*uint64_t postId = 7696582443305;
  uint64_t personId = 933; 
  auto creationDate = std::string("2010-02-14T15:32:10.447+0000");*/

  auto q1 = query(gdb).nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(uint64_props[0]); });

  auto q2 =
      query(gdb)
          .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(uint64_props[1]); })
          .crossjoin(q1)
          .create_rship({0, 1}, ":likes", {{"creationDate", boost::any(str_props[0])}});
          //.collect(rs);

  query::start({&q1, &q2});
}

//void ldbc_iu_query_3(graph_db_ptr &gdb, result_set &rs) {
void ldbc_iu_query_3(graph_db_ptr &gdb, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props, std::vector<int>& int_props) {
  
  /*uint64_t commentId = 2199026401296;
  uint64_t personId = 1564;
  auto creationDate = std::string("2012-01-23T08:56:30.617+0000");*/

  auto q1 = query(gdb).nodes_where("Comment", "id",
                            [&](auto &p) { return p.equal(uint64_props[0]); });
  auto q2 =
      query(gdb)
          .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(uint64_props[1]); })
          .crossjoin(q1)
          .create_rship({0, 1}, ":likes", {{"creationDate", boost::any(str_props[0])}});
          //.collect(rs);

  query::start({&q1, &q2});
}

//void ldbc_iu_query_4(graph_db_ptr &gdb, result_set &rs) {
void ldbc_iu_query_4(graph_db_ptr &gdb, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props, std::vector<int>& int_props) {
  /*uint64_t personId = 1564;
  uint64_t tagId = 206;
  uint64_t forumId = 53975;
  auto title = std::string("Wall of Emperor of Brazil Silva");
  auto creationDate = std::string("2010-01-02T06:05:05.320+0000");*/

  auto q1 = query(gdb).nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(uint64_props[0]); });

  auto q2 = query(gdb).nodes_where("Tag", "id",
                            [&](auto &p) { return p.equal(uint64_props[1]); });

  auto q3 = query(gdb).create("Forum",
                              {{"id", boost::any(uint64_props[2])},
                              {"title", boost::any(str_props[0])},
                              {"creationDate", boost::any(str_props[1])} })
                      .crossjoin(q1)
                      .create_rship({0, 1}, ":hasModerator", {})
                      .crossjoin(q2)
                      .create_rship({0, 3}, ":hasTag", {});
                      //.collect(rs);

  query::start({&q1, &q2, &q3});
}

//void ldbc_iu_query_5(graph_db_ptr &gdb, result_set &rs) {
void ldbc_iu_query_5(graph_db_ptr &gdb, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props, std::vector<int>& int_props) {
  /*uint64_t personId = 1564;
  uint64_t forumId = 37;
  auto joinDate = std::string("2010-02-23T09:10:25.466+0000");*/

  auto q1 = 
      query(gdb)
          .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(uint64_props[0]); });

  auto q2 =
      query(gdb)
          .nodes_where("Forum", "id",
                            [&](auto &p) { return p.equal(uint64_props[1]); })
          .crossjoin(q1)
          .create_rship({0, 1}, ":hasMember", {{"creationDate", boost::any(str_props[0])}});
          //.collect(rs);

  query::start({&q1, &q2});
}

//void ldbc_iu_query_6(graph_db_ptr &gdb, result_set &rs) {
void ldbc_iu_query_6(graph_db_ptr &gdb, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props, std::vector<int>& int_props) {
  /*uint64_t postId = 13439;  
  auto imageFile = std::string("");
  auto creationDate = std::string("2011-09-07T14:52:27.809+0000");
  auto locationIP = std::string("46.19.159.176"); 
  auto browser = std::string("Safari");
  auto language = std::string("\"uz\""); 
  auto content = std::string("About Alexander I of Russia,  (23 December  1777 – 1 December  1825), (Russian: "
                            "Александр Благословенный, Aleksandr Blagoslovennyi, meaning Alexander the Bless"); 
  auto length = 159;
  uint64_t personId = 6597069777240;
  uint64_t forumId = 2748782215183; 
  uint64_t countryId = 50;
  uint64_t tagId = 1679;*/

  auto q1 = query(gdb)
                .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(uint64_props[0]); });

  auto q2 = query(gdb).nodes_where("Forum", "id",
                            [&](auto &p) { return p.equal(uint64_props[1]); });

  auto q3 = query(gdb).nodes_where("Place", "id",
                            [&](auto &p) { return p.equal(uint64_props[2]); });

  auto q4 = query(gdb).nodes_where("Tag", "id",
                            [&](auto &p) { return p.equal(uint64_props[3]); });

  auto q5 = query(gdb).create("Post",
                            {{"id", boost::any(uint64_props[4])}, 
                              {"imageFile", boost::any(str_props[0])},
                              {"creationDate", boost::any(str_props[1])},
                              {"locationIP", boost::any(str_props[2])},
                              {"browserUsed", boost::any(str_props[3])},
                              {"language", boost::any(str_props[4])}, 
                              {"content", boost::any(str_props[5])},
                              {"length", boost::any(int_props[0])} })
                      .crossjoin(q1)
                      .create_rship({0, 1}, ":hasCreator", {})
                      .crossjoin(q2)
                      .create_rship({3, 0}, ":containerOf", {})
                      .crossjoin(q3)
                      .create_rship({0, 5}, ":isLocatedn", {})
                      .crossjoin(q4)
                      .create_rship({0, 7}, ":hasTag", {});
                      //.collect(rs);

  query::start({&q1, &q2, &q3, &q4, &q5});
}

//void ldbc_iu_query_7(graph_db_ptr &gdb, result_set &rs, std::vector<uint64_t>& uint64_props,
  //                    std::vector<std::string>& str_props, std::vector<int>& int_props) {
void ldbc_iu_query_7(graph_db_ptr &gdb, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props, std::vector<int>& int_props) {
  /*uint64_t commentId = 442214; 
  auto creationDate = std::string("2012-01-09T11:49:15.991+0000");
  auto locationIP = std::string("91.149.169.27"); 
  auto browser = std::string("Chrome");
  auto content = std::string("fine"); 
  auto length = 4;
  uint64_t personId = 10995116283243;
  uint64_t postId = 1649267442210; 
  uint64_t countryId = 63;
  uint64_t tagId = 1679;*/

  auto q1 = query(gdb).nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(uint64_props[0]); });

  auto q2 = query(gdb).nodes_where("Post", "id",
                            [&](auto &p) { return p.equal(uint64_props[1]); });

  auto q3 = query(gdb).nodes_where("Place", "id",
                            [&](auto &p) { return p.equal(uint64_props[2]); });

  auto q4 = query(gdb).nodes_where("Tag", "id",
                            [&](auto &p) { return p.equal(uint64_props[3]); });

  auto q5 = query(gdb).create("Comment",
                              {{"id", boost::any(uint64_props[4])},
                              {"creationDate", boost::any(str_props[0])},
                              {"locationIP", boost::any(str_props[1])},
                              {"browserUsed", boost::any(str_props[2])},
                              {"content", boost::any(str_props[3])},
                              {"length", boost::any(int_props[0])} })
                      .crossjoin(q1)
                      .create_rship({0, 1}, ":hasCreator", {})
                      .crossjoin(q2)
                      .create_rship({0, 3}, ":replyOf", {})
                      .crossjoin(q3)
                      .create_rship({0, 5}, ":isLocatedn", {})
                      .crossjoin(q4)
                      .create_rship({0, 7}, ":hasTag", {});
                      //.collect(rs);

  query::start({&q1, &q2, &q3, &q4, &q5});
}

//void ldbc_iu_query_8(graph_db_ptr &gdb, result_set &rs) {
void ldbc_iu_query_8(graph_db_ptr &gdb, std::vector<uint64_t>& uint64_props,
                      std::vector<std::string>& str_props, std::vector<int>& int_props) {
  /*uint64_t personId_1 = 4194;
  uint64_t personId_2 = 1564;
  auto creationDate = std::string("2010-02-23T09:10:15.466+0000");*/

  auto q1 = 
      query(gdb)
          .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(uint64_props[1]); });
  auto q2 =
      query(gdb)
          .nodes_where("Person", "id",
                            [&](auto &p) { return p.equal(uint64_props[0]); })
          .crossjoin(q1)
          .create_rship({0, 1}, ":KNOWS", {{"creationDate", boost::any(str_props[0])}});
          //.collect(rs);

  query::start({&q1, &q2});
}

void run_ldbc_queries(graph_db_ptr &gdb) {
  // the query set
  /*std::function<void(graph_db_ptr &, result_set &)> query_set[] = {
      ldbc_is_query_1, ldbc_is_query_2, ldbc_is_query_3, 
      ldbc_is_query_4, ldbc_is_query_5, ldbc_is_query_6, ldbc_is_query_7,
      ldbc_iu_query_1, ldbc_iu_query_2, ldbc_iu_query_3, ldbc_iu_query_4,
      ldbc_iu_query_5, ldbc_iu_query_6, ldbc_iu_query_7, ldbc_iu_query_8};
    
  std::size_t qnum = 1;

  // for each query we measure the time and run it in a transaction
  for (auto f : query_set) {
    result_set rs;
    auto start_qp = std::chrono::steady_clock::now();

    auto tx = gdb->begin_transaction();
    f(gdb, rs);
    gdb->commit_transaction();

    auto end_qp = std::chrono::steady_clock::now();
    std::cout << "Query #" << qnum++ << " executed in "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end_qp -
                                                                       start_qp)
                     .count()
              << " ms" << std::endl;
  }*/
}