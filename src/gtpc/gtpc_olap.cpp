#include <iostream>
#include <regex>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "defs.hpp"
#include "gtpc.hpp"
#include "config.h"

#include "threadsafe_queue.hpp"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"

namespace pj = builtin;
using namespace boost::posix_time;

static const std::vector<std::string> REGIONS =
  {"ALGERIA", "ARGENTINA", "BRAZIL", "CANADA", "EGYPT",
   "ETHIOPIA", "FRANCE", "GERMANY", "INDIA", "INDONESIA"};

/* ------------------------------------------------------------------------ */

void gtpc_olap_1(graph_db_ptr &gdb, result_set &rs) {
  auto delta = 90;
  // auto delta = gen_random_uniform_int(60, 120);
  auto max_ship_dt = time_from_string(std::string("2010-02-14 00:00:00.000"));

  query_ctx ctx(gdb);
  auto q = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("OrderLine")
              .property( "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto gdt = max_ship_dt - hours(24 * delta);
                return dt <= gdt; })
#else
              .nodes_where("OrderLine", "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto gdt = max_ship_dt - hours(24 * delta);
                return dt <= gdt; })
#endif
              .project({PExpr_(0, pj::int_property(res, "number")),
                        PExpr_(0, pj::int_property(res, "quantity")),
                        PExpr_(0, pj::double_property(res, "amount")) })
              .groupby({0}, {{"sum", 1}, {"sum", 2}, {"avg", 1}, {"avg", 2}, {"count", 0}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<int>(q1[0]) < boost::get<int>(q2[0]); })
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_2(graph_db_ptr &gdb, result_set &rs) {
  std::string data = "b";
  std::string region = "EUROPE";
  // auto region = REGIONS[gen_random_uniform_int(0, 9)];

  query_ctx ctx(gdb);

  auto q = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Region")
              .property( "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(region);
                return a == b; })
#else
              .nodes_where("Region", "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(region);
                return a == b; })
#endif
              .to_relationships(":isPartOf")
              .from_node("Nation")
              .to_relationships(":isLocatedIn")
              .from_node("Supplier")
              .to_relationships(":hasSupplier")
              .from_node("Stock")
              .to_relationships(":hasStock")
              .from_node("Item")
              .property("data", [&](auto &prop) {
                auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto s = gdb->get_dictionary()->lookup_code(c);
                std::regex r(".*" + data);
                return std::regex_match(s, r); })
              .project({PVar_(8),
                        PExpr_(6, pj::int_property(res, "quantity")) })
              .groupby({0}, {{"min", 1}})
              .from_relationships(":hasStock", 0)
              .to_node("Stock")
              .project({PVar_(0),
                        PVar_(1),
                        PVar_(3),
                        PExpr_(3, pj::int_property(res, "quantity")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<int>(v[1]) == boost::get<int>(v[3]); })
              .from_relationships(":hasSupplier", 2)
              .to_node("Supplier")
              .from_relationships(":isLocatedIn")
              .to_node("Nation")
              .project({PExpr_(5, pj::uint64_property(res, "id")),
                        PExpr_(5, pj::string_property(res, "name")),
                        PExpr_(7, pj::string_property(res, "name")),
                        PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::string_property(res, "name")),
                        PExpr_(5, pj::string_property(res, "address")),
                        PExpr_(5, pj::uint64_property(res, "phone")),
                        PExpr_(5, pj::string_property(res, "comment")) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<std::string>(q1[2]) == boost::get<std::string>(q2[2])) {
                  if (boost::get<std::string>(q1[1]) == boost::get<std::string>(q2[1]))
                    return boost::get<uint64_t>(q1[3]) < boost::get<uint64_t>(q2[3]);
                  return boost::get<std::string>(q1[1]) < boost::get<std::string>(q2[1]);
                }
                return boost::get<std::string>(q1[2]) < boost::get<std::string>(q2[2]); })
              .limit(100)
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_3(graph_db_ptr &gdb, result_set &rs) {
  std::string state = "A";
  auto date = time_from_string(std::string("2011-10-30 00:00:00.000"));

  query_ctx ctx(gdb);

  auto q = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Customer")
              .property( "state", [&](auto &prop) {
                auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto s = gdb->get_dictionary()->lookup_code(c);
                std::regex r(state + ".*");
                return std::regex_match(s, r); })
#else
              .nodes_where("Customer", "state", [&](auto &prop) {
                auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto s = gdb->get_dictionary()->lookup_code(c);
                std::regex r(state + ".*");
                return std::regex_match(s, r); })
#endif
              .from_relationships(":hasPlaced")
              .to_node("Order")
              .property("entry_d", [&](auto &prop) {
                return (*(reinterpret_cast<const ptime *>(prop.value_))) < date; })
              .from_relationships(":contains")
              .to_node("OrderLine")
              .property("delivery_d", [&](auto &prop) {
                return (*(reinterpret_cast<const ptime *>(prop.value_))) > date; })
              .project({PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(4, pj::double_property(res, "amount")),
                        PExpr_(2, pj::ptime_property(res, "entry_d")) })
              .groupby({0, 2}, {{"sum", 1}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<double>(q1[2]) == boost::get<double>(q2[2]))
                  return boost::get<ptime>(q1[1]) < boost::get<ptime>(q2[1]);
                return boost::get<double>(q1[2]) > boost::get<double>(q2[2]); })
              .limit(10)
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_4(graph_db_ptr &gdb, result_set &rs) {
  auto sqtr = time_from_string(std::string("2011-08-01 00:00:00.000"));

  query_ctx ctx(gdb);

  auto q = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Order")
              .property( "entry_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto eqtr = sqtr + hours(24 * 30 * 3);
                return sqtr <= dt && dt < eqtr; })
#else
              .nodes_where("Order", "entry_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto eqtr = sqtr + hours(24 * 30 * 3);
                return sqtr <= dt && dt < eqtr; })
#endif
              .from_relationships(":contains")
              .to_node("OrderLine")
              .project({PExpr_(0, pj::ptime_property(res, "entry_d")),
                        PExpr_(2, pj::ptime_property(res, "delivery_d")),
                        PExpr_(0, pj::int_property(res, "ol_cnt")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<ptime>(v[0]) < boost::get<ptime>(v[1]); })
              .groupby({2}, {{"count", 0}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<int>(q1[0]) < boost::get<int>(q2[0]); })
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_5(graph_db_ptr &gdb, result_set &rs) {
  std::string region = "ASIA";
  // auto region = REGIONS[gen_random_uniform_int(0, 9)];
  auto sdt = time_from_string(std::string("2010-01-01 00:00:00.000"));

  query_ctx ctx(gdb);

  auto q = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Region")
              .property( "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(region);
                return a == b; })
#else
              .nodes_where("Region", "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(region);
                return a == b; })
#endif
              .to_relationships(":isPartOf")
              .from_node("Nation")
              .to_relationships(":isLocatedIn")
              .from_node("Customer")
              .from_relationships(":hasPlaced")
              .to_node("Order")
              .property( "entry_d", [&](auto &prop) {
                auto dt = *(reinterpret_cast<const ptime *>(prop.value_));
                auto edt = sdt + hours(24 * 365);
                return sdt <= dt && dt < edt; })
              .from_relationships(":contains")
              .to_node("OrderLine")
              .from_relationships(":hasStock")
              .to_node("Stock")
              .from_relationships(":hasSupplier")
              .to_node("Supplier")
              .from_relationships(":isLocatedIn")
              .to_node("Nation")
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<node *>(v[2])->id() == boost::get<node *>(v[14])->id(); })
              .project({PExpr_(2, pj::string_property(res, "name")),
                        PExpr_(8, pj::double_property(res, "amount")) })
              .groupby({0}, {{"sum", 1}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<double>(q1[1]) > boost::get<double>(q2[1]); })
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_6(graph_db_ptr &gdb, result_set &rs) {
  auto sdt = time_from_string(std::string("2011-04-01 00:00:00.000"));
  // auto quantity = gen_random_uniform_int(24, 25);
  auto quantity = 5;

  query_ctx ctx(gdb);

  auto q = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("OrderLine")
              .property( "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto sdt = date;
                auto edt = sdt + hours(24 * 365);
                return sdt <= dt && dt < edt; })
#else
              .nodes_where("OrderLine", "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = sdt + hours(24 * 365);
                return sdt <= dt && dt < edt; })
#endif
              .property( "quantity", [&](auto &prop) {
                return (*(reinterpret_cast<const int *>(prop.value_))) == quantity; })
              .project({PExpr_(0, pj::double_property(res, "amount")) })
              .groupby({}, {{"sum", 0}})
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_7(graph_db_ptr &gdb, result_set &rs) {
  std::string nation1 = "FRANCE";
  std::string nation2 = "GERMANY";
  auto sdt = time_from_string(std::string("2011-01-01 00:00:00.000"));
  auto edt = time_from_string(std::string("2012-01-01 00:00:00.000"));

  query_ctx ctx(gdb);

  auto q1 = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Nation")
              .property( "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(nation1);
                return a == b; })
#else
              .nodes_where("Nation", "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(nation1);
                return a == b; })
#endif
              .to_relationships(":isLocatedIn")
              .from_node("Customer")
              .from_relationships(":hasPlaced")
              .to_node("Order")
              .from_relationships(":contains")
              .to_node("OrderLine")
              .property( "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                return sdt <= dt && dt < edt; })
              .from_relationships(":hasStock")
              .to_node("Stock")
              .from_relationships(":hasSupplier")
              .to_node("Supplier")
              .from_relationships(":isLocatedIn")
              .to_node("Nation")
              .property( "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(nation2);
                return a == b; })
              .project({PExpr_(12, pj::string_property(res, "name")),
                        PExpr_(0, pj::string_property(res, "name")),
                        PExpr_(6, pj::pr_year(res, "delivery_d")),
                        PExpr_(6, pj::double_property(res, "amount")) });

    auto q2 = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Nation")
              .property( "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(nation2);
                return a == b; })
#else
              .nodes_where("Nation", "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(nation2);
                return a == b; })
#endif
              .to_relationships(":isLocatedIn")
              .from_node("Customer")
              .from_relationships(":hasPlaced")
              .to_node("Order")
              .from_relationships(":contains")
              .to_node("OrderLine")
              .property( "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                return sdt <= dt && dt < edt; })
              .from_relationships(":hasStock")
              .to_node("Stock")
              .from_relationships(":hasSupplier")
              .to_node("Supplier")
              .from_relationships(":isLocatedIn")
              .to_node("Nation")
              .property( "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(nation1);
                return a == b; })
              .project({PExpr_(12, pj::string_property(res, "name")),
                        PExpr_(0, pj::string_property(res, "name")),
                        PExpr_(6, pj::pr_year(res, "delivery_d")),
                        PExpr_(6, pj::double_property(res, "amount")) })
              .union_all({&q1})
              .groupby({0, 1, 2}, {{"sum", 3}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<std::string>(q1[0]) == boost::get<std::string>(q2[0])) {
                  if (boost::get<std::string>(q1[1]) == boost::get<std::string>(q2[1]))
                    return boost::get<int>(q1[2]) < boost::get<int>(q2[2]);
                  return boost::get<std::string>(q1[1]) < boost::get<std::string>(q2[1]);
                }
                return boost::get<std::string>(q1[0]) < boost::get<std::string>(q2[0]); })
              .collect(rs);
    query::start({&q1, &q2});
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_8(graph_db_ptr &gdb, result_set &rs) {
  // auto region = REGIONS[gen_random_uniform_int(0, 9)];
  std::string region = "EUROPE";
  std::string nation = "BRAZIL";
  std::string data = "b";
  auto sdt = time_from_string(std::string("2011-01-01 00:00:00.000"));
  auto edt = time_from_string(std::string("2012-01-01 00:00:00.000"));

  query_ctx ctx(gdb);

  auto q = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Region")
              .property( "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(region);
                return a == b; })
#else
              .nodes_where("Region", "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(region);
                return a == b; })
#endif
              .to_relationships(":isPartOf")
              .from_node("Nation")
              .to_relationships(":isLocatedIn")
              .from_node("Customer")
              .from_relationships(":hasPlaced")
              .to_node("Order")
              .property( "entry_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                return sdt <= dt && dt < edt; })
              .from_relationships(":contains")
              .to_node("OrderLine")
              .from_relationships(":hasStock")
              .to_node("Stock")
              .to_relationships(":hasStock")
              .from_node("Item")
              .property("data", [&](auto &prop) {
                auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto s = gdb->get_dictionary()->lookup_code(c);
                std::regex r(".*" + data);
                return std::regex_match(s, r); })
              .from_relationships(":hasSupplier", 10)
              .to_node("Supplier")
              .from_relationships(":isLocatedIn")
              .to_node("Nation")
              .project({PExpr_(6, pj::pr_year(res, "entry_d")),
                        PExpr_(16, pj::string_property(res, "name")),
                        PExpr_(8, pj::double_property(res, "amount")) })
              .append_to_qr_tuple([&](const qr_tuple &v) {
                bool b = boost::get<std::string>(v[1]) == nation;
                return b ? boost::get<double>(v[2]) : 0.0; })
              .groupby({0}, {{"sum", 2}, {"sum", 3}})
              .append_to_qr_tuple([&](const qr_tuple &v) {
                return boost::get<double>(v[2]) / (double)boost::get<double>(v[1]); })
              .project({PVar_(0),
                        PVar_(3)})
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_9(graph_db_ptr &gdb, result_set &rs) {
  std::string data = "BB";

  query_ctx ctx(gdb);

  auto q = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Item")
              .property( "data", [&](auto &prop) {
                auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto s = gdb->get_dictionary()->lookup_code(c);
                std::regex r(".*" + data);
                return std::regex_match(s, r); })
#else
              .nodes_where("Item", "data", [&](auto &prop) {
                auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto s = gdb->get_dictionary()->lookup_code(c);
                std::regex r(".*" + data);
                return std::regex_match(s, r); })
#endif
              .from_relationships(":hasStock")
              .to_node("Stock")
              .to_relationships(":hasStock")
              .from_node("OrderLine")
              .to_relationships(":contains")
              .from_node("Order")
              .from_relationships(":hasSupplier", 2)
              .to_node("Supplier")
              .from_relationships(":isLocatedIn")
              .to_node("Nation")
              .project({PExpr_(10, pj::string_property(res, "name")),
                        PExpr_(6, pj::pr_year(res, "entry_d")),
                        PExpr_(4, pj::double_property(res, "amount")) })
              .groupby({0, 1}, {{"sum", 2}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<std::string>(q1[0]) == boost::get<std::string>(q2[0]))
                    return boost::get<int>(q1[1]) > boost::get<int>(q2[1]);
                return boost::get<std::string>(q1[0]) < boost::get<std::string>(q2[0]); })
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_10(graph_db_ptr &gdb, result_set &rs) {
  auto sdt = time_from_string(std::string("2011-08-01 00:00:00.000"));

  query_ctx ctx(gdb);

  auto q = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Order")
              .property( "entry_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = sdt + hours(24 * 30 * 3);
                return sdt <= dt && dt < edt; })
#else
              .nodes_where("Order", "entry_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = sdt + hours(24 * 30 * 3);
                return sdt <= dt && dt < edt; })
#endif
              .to_relationships(":hasPlaced")
              .from_node("Customer")
              .from_relationships(":isLocatedIn")
              .to_node("Nation")
              .from_relationships(":contains", 0)
              .to_node("OrderLine")
              .project({PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(2, pj::string_property(res, "last")),
                        PExpr_(6, pj::double_property(res, "amount")),
                        PExpr_(2, pj::string_property(res, "city")),
                        PExpr_(2, pj::uint64_property(res, "phone")),
                        PExpr_(4, pj::string_property(res, "name"))})
              .groupby({0, 1, 3, 4, 5}, {{"sum", 2}})
              .project({PVar_(0),
                        PVar_(1),
                        PVar_(5),
                        PVar_(2),
                        PVar_(3),
                        PVar_(4)})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<double>(q1[2]) > boost::get<double>(q2[2]); })
              .limit(20)
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_11(graph_db_ptr &gdb, result_set &rs) {
  std::string nation = "GERMANY";
  double fraction = 0.0001;

  query_ctx ctx(gdb);

  auto q = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Nation")
              .property( "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(nation);
                return a == b; })
#else
              .nodes_where("Nation", "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(nation);
                return a == b; })
#endif
              .to_relationships(":isLocatedIn")
              .from_node("Supplier")
              .to_relationships(":hasSupplier")
              .from_node("Stock")
              .project({PExpr_(4, pj::uint64_property(res, "id")),
                        PExpr_(4, pj::int_property(res, "order_cnt"))})
              .groupby({0}, {{"sum", 1}})
              .where_qr_tuple([&](const qr_tuple &v) {
                auto sum = boost::get<int>(v[1]);
                return sum >= (sum * fraction); })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<int>(q1[1]) > boost::get<int>(q2[1]); })
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_12(graph_db_ptr &gdb, result_set &rs) {
  auto sdt = time_from_string(std::string("2011-04-01 00:00:00.000"));

  query_ctx ctx(gdb);

  auto q = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("OrderLine")
              .property( "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = sdt + hours(24 * 365);
                return sdt <= dt && dt < edt; })
#else
              .nodes_where("OrderLine", "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = sdt + hours(24 * 365);
                return sdt <= dt && dt < edt; })
#endif
              .to_relationships(":contains")
              .from_node("Order")
              .project({PExpr_(2, pj::int_property(res, "ol_cnt")),
                        PExpr_(2, pj::int_property(res, "carrier_id")),
                        PExpr_(2, pj::ptime_property(res, "entry_d")),
                        PExpr_(0, pj::ptime_property(res, "delivery_d"))})
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<ptime>(v[2]) < boost::get<ptime>(v[3]); })
              .append_to_qr_tuple([&](const qr_tuple &v) {
                auto carrier_id = boost::get<int>(v[1]);
                return (carrier_id == 1 || carrier_id == 2) ?
                        query_result(1) : query_result(0); })
              .append_to_qr_tuple([&](const qr_tuple &v) {
                auto carrier_id = boost::get<int>(v[1]);
                return (carrier_id != 1 && carrier_id != 2) ?
                        query_result(1) : query_result(0); })
              .groupby({0}, {{"sum", 4}, {"sum", 5}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<int>(q1[0]) < boost::get<int>(q2[0]); })
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_13(graph_db_ptr &gdb, result_set &rs) {
  auto carrier_id = 8;

  query_ctx ctx(gdb);

  auto q1 = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Order")
              .property("carrier_id", [&](auto &prop) {
                return (*(reinterpret_cast<const int *>(prop.value_))) > carrier_id; });
#else
              .nodes_where("Order", "carrier_id", [&](auto &prop) {
                return (*(reinterpret_cast<const int *>(prop.value_))) > carrier_id; });
#endif

    auto q2 = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Customer")
#else
              .all_nodes("Customer")
#endif
              .outerjoin(q1, [&](const qr_tuple &lv, const qr_tuple &rv) {
                auto connected = false;
                auto src = boost::get<node *>(lv[0]);
                auto des = boost::get<node *>(rv[0]);
                ctx.foreach_from_relationship_of_node((*src), [&](auto &r) {
                  if (r.to_node_id() == des->id())
                    connected = true;
                });
                return connected; })
              .where_qr_tuple([&](const qr_tuple &v) {
                return v[1].type() == typeid(null_val); })
              .append_to_qr_tuple([&](const qr_tuple &v) {
                return query_result((uint64_t)0); })
              .project({PVar_(0),
                        PVar_(2)});

    auto q3 = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Order")
              .property("carrier_id", [&](auto &prop) {
                return (*(reinterpret_cast<const int *>(prop.value_))) > carrier_id; })
#else
              .nodes_where("Order", "carrier_id", [&](auto &prop) {
                return (*(reinterpret_cast<const int *>(prop.value_))) > carrier_id; })
#endif
              .to_relationships(":hasPlaced")
              .from_node("Customer")
              .groupby({2}, {{"count", 0}})
              .union_all({&q2})
              .groupby({1}, {{"count", 0}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[1]) == boost::get<uint64_t>(q2[1]))
                  return boost::get<uint64_t>(q1[0]) > boost::get<uint64_t>(q2[0]);
                return boost::get<uint64_t>(q1[1]) > boost::get<uint64_t>(q2[1]); })
              .collect(rs);
    query::start({&q1, &q2, &q3});
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_14(graph_db_ptr &gdb, result_set &rs) {
  auto sdt = time_from_string(std::string("2011-04-01 00:00:00.000"));
  std::string data = "PR";

  query_ctx ctx(gdb);

  auto q = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("OrderLine")
              .property( "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = sdt + hours(24 * 30);
                return sdt <= dt && dt < edt; })
#else
              .nodes_where("OrderLine", "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = sdt + hours(24 * 30);
                return sdt <= dt && dt < edt; })
#endif
              .from_relationships(":hasStock")
              .to_node("Stock")
              .to_relationships(":hasStock")
              .from_node("Item")
              .project({PExpr_(4, pj::string_property(res, "data")),
                        PExpr_(0, pj::double_property(res, "amount"))})
              .append_to_qr_tuple([&](const qr_tuple &v) {
                auto s = boost::get<std::string>(v[0]);
                std::regex r(data + ".*");
                return std::regex_match(s, r) ? v[1] : query_result(0.0); })
              .groupby({}, {{"sum", 2}, {"sum", 1}})
              .append_to_qr_tuple([&](const qr_tuple &v) {
                auto sum1 = boost::get<double>(v[0]);
                auto sum2 = boost::get<double>(v[1]);
                auto promo_revenue = (sum2 == 0) ? 0.0 : 100 * sum1 / sum2;
                return query_result(promo_revenue); })
              .project({PVar_(2)})
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_15(graph_db_ptr &gdb, result_set &rs) {
  auto sdt = time_from_string(std::string("2011-04-01 00:00:00.000"));

  query_ctx ctx(gdb);

  auto q1 = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("OrderLine")
              .property( "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = sdt + hours(24 * 30 * 3);
                return sdt <= dt && dt < edt; })
#else
              .nodes_where("OrderLine", "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = sdt + hours(24 * 30 * 3);
                return sdt <= dt && dt < edt; })
#endif
              .from_relationships(":hasStock")
              .to_node("Stock")
              .from_relationships(":hasSupplier")
              .to_node("Supplier")
              .project({PVar_(4),
                        PExpr_(0, pj::double_property(res, "amount"))})
              .groupby({0}, {{"sum", 1}})
              .groupby({}, {{"max", 1}});

    auto q2 = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("OrderLine")
              .property( "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = sdt + hours(24 * 30 * 3);
                return sdt <= dt && dt < edt; })
#else
              .nodes_where("OrderLine", "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto edt = sdt + hours(24 * 30 * 3);
                return sdt <= dt && dt < edt; })
#endif
              .from_relationships(":hasStock")
              .to_node("Stock")
              .from_relationships(":hasSupplier")
              .to_node("Supplier")
              .project({PVar_(4),
                        PExpr_(0, pj::double_property(res, "amount"))})
              .groupby({0}, {{"sum", 1}})
              .project({PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::string_property(res, "name")),
                        PExpr_(0, pj::string_property(res, "address")),
                        PExpr_(0, pj::uint64_property(res, "phone")),
                        PVar_(1) })
              .crossjoin(q1)
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<double>(v[4]) == boost::get<double>(v[5]); })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]); })
              .collect(rs);
    query::start({&q1, &q2});
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_16(graph_db_ptr &gdb, result_set &rs) {
  std::string data = "zz";
  std::string comment = "bad";

  query_ctx ctx(gdb);
    auto q = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Item")
              .property( "data", [&](auto &prop) {
                auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto s = gdb->get_dictionary()->lookup_code(c);
                std::regex r(data + ".*");
                return !std::regex_match(s, r); })
#else
              .nodes_where("Item", "data", [&](auto &prop) {
                auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto s = gdb->get_dictionary()->lookup_code(c);
                std::regex r(data + ".*");
                return !std::regex_match(s, r); })
#endif
              .from_relationships(":hasStock")
              .to_node("Stock")
              .from_relationships(":hasSupplier")
              .to_node("Supplier")
              .property( "comment", [&](auto &prop) {
                auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto s = gdb->get_dictionary()->lookup_code(c);
                std::regex r(".*" + comment + ".*");
                return !std::regex_match(s, r); })
              .project({PExpr_(0, pj::string_property(res, "name")),
                        projection::expr(0, [&](query_ctx& ctx, const query_result& res) {
                          auto str = boost::get<std::string>(pj::string_property(res, "data"));
                          auto brand = str.substr(1, 3);
                          return query_result(brand); }),
                        PExpr_(0, pj::double_property(res, "price")),
                        PExpr_(4, pj::uint64_property(res, "id")) })
              .distinct()
              .groupby({0, 1, 2}, {{"count", 0}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<uint64_t>(q1[3]) > boost::get<uint64_t>(q2[3]); })
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_17(graph_db_ptr &gdb, result_set &rs) {
  std::string data = "b";

  query_ctx ctx(gdb);
    auto q = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Item")
              .property( "data", [&](auto &prop) {
                auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto s = gdb->get_dictionary()->lookup_code(c);
                std::regex r(".*" + data);
                return !std::regex_match(s, r); })
#else
              .nodes_where("Item", "data", [&](auto &prop) {
                auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto s = gdb->get_dictionary()->lookup_code(c);
                std::regex r(".*" + data);
                return !std::regex_match(s, r); })
#endif
              .from_relationships(":hasStock")
              .to_node("Stock")
              .to_relationships(":hasStock")
              .from_node("OrderLine")
              .project({PVar_(0),
                        PExpr_(4, pj::int_property(res, "quantity"))})
              .groupby({0}, {{"avg", 1}})
              .from_relationships(":hasStock", 0)
              .to_node("Stock")
              .to_relationships(":hasStock")
              .from_node("OrderLine")
              .project({PExpr_(5, pj::int_property(res, "quantity")),
                        PExpr_(5, pj::double_property(res, "amount")),
                        PVar_(1) })
              .where_qr_tuple([&](const qr_tuple &v) {
                auto qty = boost::get<int>(v[0]);
                auto avg_qty = boost::get<double>(v[2]);
                return qty <= avg_qty; })
              .groupby({}, {{"sum", 1}})
              .append_to_qr_tuple([&](const qr_tuple &v) {
                auto sum = boost::get<double>(v[0]);
                auto yearly_avg = sum / (double)2.0;
                return query_result(yearly_avg); })
              .project({PVar_(1)})
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_18(graph_db_ptr &gdb, result_set &rs) {
  auto amount = 100;

  query_ctx ctx(gdb);
    auto q = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Order")
#else
              .all_nodes("Order")
#endif
              .from_relationships(":contains")
              .to_node("OrderLine")
              .project({PVar_(0),
                        PExpr_(2, pj::double_property(res, "amount")) })
              .groupby({0}, {{"sum", 1}})
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<double>(v[1]) > amount; })
              .from_relationships(":contains", 0)
              .to_node("OrderLine")
              .to_relationships(":hasPlaced", 0)
              .from_node("Customer")
              .project({PExpr_(5, pj::string_property(res, "last")),
                        PExpr_(5, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::uint64_property(res, "id")),
                        PExpr_(0, pj::ptime_property(res, "entry_d")),
                        PExpr_(0, pj::int_property(res, "ol_cnt")),
                        PExpr_(3, pj::double_property(res, "amount")) })
              .groupby({0, 1, 2, 3, 4}, {{"sum", 5}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<double>(q1[5]) == boost::get<double>(q2[5]))
                  return boost::get<ptime>(q1[3]) < boost::get<ptime>(q2[3]);
                return boost::get<double>(q1[5]) > boost::get<double>(q2[5]); })
              .limit(100)
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_19(graph_db_ptr &gdb, result_set &rs) {
  std::string data1 = "a";
  std::string data2 = "b";
  std::string data3 = "c";
  auto price1 = 1;
  auto price2 = 400000;
  auto quantity1 = 1;
  auto quantity2 = 10;

  query_ctx ctx(gdb);
    auto q1 = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Item")
#else
              .all_nodes("Item")
#endif
              .project({PVar_(0),
                        PExpr_(0, pj::string_property(res, "data")),
                        PExpr_(0, pj::double_property(res, "price"))})
              .where_qr_tuple([&](const qr_tuple &v) {
                auto s = boost::get<std::string>(v[1]);
                std::regex r(".*" + data1);
                auto pred1 = std::regex_match(s, r);
                auto p = boost::get<double>(v[2]);
                auto pred2 = price1 <= p && p <= price2;
                return pred1 && pred2; })
              .from_relationships(":hasStock", 0)
              .to_node("Stock")
              .to_relationships(":hasStock")
              .from_node("OrderLine")
              .project({PExpr_(6, pj::int_property(res, "quantity")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                auto amount = boost::get<int>(v[0]);
                return quantity1 <= amount && amount <= quantity2; });

   auto q2 = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Item")
#else
              .all_nodes("Item")
#endif
              .project({PVar_(0),
                        PExpr_(0, pj::string_property(res, "data")),
                        PExpr_(0, pj::double_property(res, "price"))})
              .where_qr_tuple([&](const qr_tuple &v) {
                auto s = boost::get<std::string>(v[1]);
                std::regex r(".*" + data2);
                auto pred1 = std::regex_match(s, r);
                auto p = boost::get<double>(v[2]);
                auto pred2 = price1 <= p && p <= price2;
                return pred1 && pred2; })
              .from_relationships(":hasStock", 0)
              .to_node("Stock")
              .to_relationships(":hasStock")
              .from_node("OrderLine")
              .project({PExpr_(6, pj::int_property(res, "quantity")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                auto amount = boost::get<int>(v[0]);
                return quantity1 <= amount && amount <= quantity2; });

    auto q3 = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Item")
#else
              .all_nodes("Item")
#endif
              .project({PVar_(0),
                        PExpr_(0, pj::string_property(res, "data")),
                        PExpr_(0, pj::double_property(res, "price"))})
              .where_qr_tuple([&](const qr_tuple &v) {
                auto s = boost::get<std::string>(v[1]);
                std::regex r(".*" + data3);
                auto pred1 = std::regex_match(s, r);
                auto p = boost::get<double>(v[2]);
                auto pred2 = price1 <= p && p <= price2;
                return pred1 && pred2; })
              .from_relationships(":hasStock", 0)
              .to_node("Stock")
              .to_relationships(":hasStock")
              .from_node("OrderLine")
              .project({PExpr_(6, pj::int_property(res, "quantity")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                auto amount = boost::get<int>(v[0]);
                return quantity1 <= amount && amount <= quantity2; })
              .union_all({&q1, &q2})
              .groupby({}, {{"sum", 0}})
              .collect(rs);
    query::start({&q1, &q2, &q3});
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_20(graph_db_ptr &gdb, result_set &rs) {
  std::string data = "co";
  auto date = time_from_string(std::string("2011-10-30 00:00:00.000"));
  std::string nation = "CHINA";

  query_ctx ctx(gdb);
    auto q = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Item")
              .property( "data", [&](auto &prop) {
                auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto s = gdb->get_dictionary()->lookup_code(c);
                std::regex r(data + ".*");
                return std::regex_match(s, r); })
#else
              .nodes_where("Item", "data", [&](auto &prop) {
                auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto s = gdb->get_dictionary()->lookup_code(c);
                std::regex r(data + ".*");
                return std::regex_match(s, r); })
#endif
              .from_relationships(":hasStock")
              .to_node("Stock")
              .to_relationships(":hasStock")
              .from_node("OrderLine")
              .property("delivery_d", [&](auto &prop) {
                return (*(reinterpret_cast<const ptime *>(prop.value_))) > date; })
              .project({PVar_(2),
                        PExpr_(2, pj::int_property(res, "quantity")),
                        PExpr_(4, pj::int_property(res, "quantity")) })
              .groupby({0, 1}, {{"sum", 2}})
              .where_qr_tuple([&](const qr_tuple &v) {
                auto s_qtty = boost::get<int>(v[1]);
                auto ol_qtty = boost::get<int>(v[2]);
                return (2 * s_qtty) > ol_qtty; })
              .from_relationships(":hasSupplier", 0)
              .to_node("Supplier")
              .from_relationships(":isLocatedIn")
              .to_node("Nation")
              .property( "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(nation);
                return a == b; })
              .project({PExpr_(4, pj::string_property(res, "name")),
                        PExpr_(4, pj::string_property(res, "address")) })
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<std::string>(q1[0]) < boost::get<std::string>(q2[0]); })
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_21(graph_db_ptr &gdb, result_set &rs) {
  std::string nation = "SAUDI ARABIA";

  query_ctx ctx(gdb);
    auto q1 = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Order")
#else
              .all_nodes("Order")
#endif
              .from_relationships(":contains")
              .to_node("OrderLine")
              .project({PVar_(0),
                        PVar_(2),
                        PExpr_(2, pj::ptime_property(res, "delivery_d")) });

    auto q2 = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Nation")
              .property( "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(nation);
                return a == b; })
#else
              .nodes_where("Nation", "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(nation);
                return a == b; })
#endif
              .to_relationships(":isLocatedIn")
              .from_node("Supplier")
              .to_relationships(":hasSupplier")
              .from_node("Stock")
              .to_relationships(":hasStock")
              .from_node("OrderLine")
              .to_relationships(":contains")
              .from_node("Order")
              .project({PVar_(2),
                        PVar_(4),
                        PVar_(6),
                        PVar_(8),
                        PExpr_(8, pj::ptime_property(res, "entry_d")),
                        PExpr_(6, pj::ptime_property(res, "delivery_d")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<ptime>(v[5]) > boost::get<ptime>(v[4]); })
              .hashjoin_on_node({3, 0}, q1)
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<ptime>(v[5]) < boost::get<ptime>(v[8]);})
              .groupby({0, 1, 2, 3});

    auto q3 = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Nation")
              .property( "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(nation);
                return a == b; })
#else
              .nodes_where("Nation", "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(nation);
                return a == b; })
#endif
              .to_relationships(":isLocatedIn")
              .from_node("Supplier")
              .to_relationships(":hasSupplier")
              .from_node("Stock")
              .to_relationships(":hasStock")
              .from_node("OrderLine")
              .to_relationships(":contains")
              .from_node("Order")
              .project({PVar_(2),
                        PVar_(4),
                        PVar_(6),
                        PVar_(8),
                        PExpr_(8, pj::ptime_property(res, "entry_d")),
                        PExpr_(6, pj::ptime_property(res, "delivery_d")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                return boost::get<ptime>(v[5]) > boost::get<ptime>(v[4]); })
              .outerjoin(q2, [&](const qr_tuple &lv, const qr_tuple &rv) {
                return boost::get<node *>(lv[2])->id() == boost::get<node *>(rv[2])->id(); })
              .where_qr_tuple([&](const qr_tuple &v) {
                return v[6].type() == typeid(null_val); })
              .project({PExpr_(0, pj::string_property(res, "name")),
                        PVar_(1) })
              .groupby({0}, {{"count", 0}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<uint64_t>(q1[1]) == boost::get<uint64_t>(q2[1]))
                  return boost::get<std::string>(q1[0]) < boost::get<std::string>(q2[0]);
                return boost::get<uint64_t>(q1[1]) > boost::get<uint64_t>(q2[1]); })
              .limit(100)
              .collect(rs);
    query::start({&q1, &q2, &q3});
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpc_olap_22(graph_db_ptr &gdb, result_set &rs) {
    std::vector<std::string> ph = {"1", "2", "3", "4", "5", "6", "7"};

  query_ctx ctx(gdb);
    auto q1 = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Customer")
#else
              .all_nodes("Customer")
#endif
              .project({PExpr_(0, pj::double_property(res, "balance")),
                        PExpr_(0, pj::uint64_property(res, "phone")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                // auto pred1 = boost::get<double>(v[0]) > 0.0;
                auto pred1 = boost::get<double>(v[0]) < 0.0;
                auto phone = boost::get<uint64_t>(v[1]);
                auto str = std::to_string(phone).substr(1, 1);
                auto pred2 = (str == ph[0]) || (str == ph[1]) ||
                              (str == ph[2]) || (str == ph[3]) ||
                              (str == ph[4]) || (str == ph[5]) ||
                              (str == ph[6]);
                return pred1 && pred2; })
              .groupby({}, {{"avg", 0}});

    auto q2 = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Customer")
#else
              .all_nodes("Customer")
#endif
              .project({PVar_(0),
                        PExpr_(0, pj::double_property(res, "balance")),
                        PExpr_(0, pj::uint64_property(res, "phone")) })
              .crossjoin(q1)
              .where_qr_tuple([&](const qr_tuple &v) {
                // auto pred1 = boost::get<double>(v[0]) > boost::get<double>(v[0]);
                auto pred1 = boost::get<double>(v[1]) >= boost::get<double>(v[3]);
                auto phone = boost::get<uint64_t>(v[2]);
                auto str = std::to_string(phone).substr(1, 1);
                auto pred2 = (str == ph[0]) || (str == ph[1]) ||
                              (str == ph[2]) || (str == ph[3]) ||
                              (str == ph[4]) || (str == ph[5]) ||
                              (str == ph[6]);
                return pred1 && pred2; })
              .from_relationships(":hasPlaced", 0)
              .to_node("Order");

    auto q3 = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Customer")
#else
              .all_nodes("Customer")
#endif
              .project({PExpr_(0, pj::double_property(res, "balance")),
                        PExpr_(0, pj::uint64_property(res, "phone")) })
              .where_qr_tuple([&](const qr_tuple &v) {
                // auto pred1 = boost::get<double>(v[0]) > 0.0;
                auto pred1 = boost::get<double>(v[0]) < 0.0;
                auto phone = boost::get<uint64_t>(v[1]);
                auto str = std::to_string(phone).substr(1, 1);
                auto pred2 = (str == ph[0]) || (str == ph[1]) ||
                              (str == ph[2]) || (str == ph[3]) ||
                              (str == ph[4]) || (str == ph[5]) ||
                              (str == ph[6]);
                return pred1 && pred2; })
              .groupby({}, {{"avg", 0}});

    auto q4 = query(ctx)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Customer")
#else
              .all_nodes("Customer")
#endif
              .project({PVar_(0),
                        PExpr_(0, pj::double_property(res, "balance")),
                        PExpr_(0, pj::uint64_property(res, "phone")) })
              .crossjoin(q3)
              .where_qr_tuple([&](const qr_tuple &v) {
                // auto pred1 = boost::get<double>(v[0]) > boost::get<double>(v[0]);
                auto pred1 = boost::get<double>(v[1]) >= boost::get<double>(v[3]);
                auto phone = boost::get<uint64_t>(v[2]);
                auto str = std::to_string(phone).substr(1, 1);
                auto pred2 = (str == ph[0]) || (str == ph[1]) ||
                              (str == ph[2]) || (str == ph[3]) ||
                              (str == ph[4]) || (str == ph[5]) ||
                              (str == ph[6]);
                return pred1 && pred2; })
              .outerjoin(q2, [&](const qr_tuple &lv, const qr_tuple &rv) {
                return boost::get<node *>(lv[0])->id() == boost::get<node *>(rv[0])->id(); })
              .where_qr_tuple([&](const qr_tuple &v) {
                // return v[4].type() == typeid(null_val);
                return v[4].type() != typeid(null_val); })
              .project({projection::expr(0, [&](query_ctx& cx, const query_result& res) {
                          auto str = boost::get<std::string>(pj::string_property(res, "state"));
                          auto sub = str.substr(1, 1);
                          return query_result(sub); }),
                        PExpr_(0, pj::double_property(res, "balance"))})
              .groupby({0}, {{"count", 0}, {"sum", 1}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<std::string>(q1[0]) < boost::get<std::string>(q2[0]); })
              .collect(rs);
    query::start({&q1, &q2, &q3, &q4});
    rs.wait();
}