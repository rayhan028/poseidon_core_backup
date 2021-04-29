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


/* ------------------------------------------------------------------------ */

void gtpch_query_1(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("OrderLine")
              .property( "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto gdt = boost::get<ptime>(params[0]) - hours(24 * boost::get<int>(params[1]));
                return dt <= gdt; })
#else
              .nodes_where("OrderLine", "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto gdt = boost::get<ptime>(params[0]) - hours(24 * boost::get<int>(params[1]));
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

void gtpch_query_2(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Region")
              .property( "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1]));
                return a == b; })
#else
              .nodes_where("Region", "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[1]));
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
                std::regex r(".*" + boost::get<std::string>(params[0]));
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
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpch_query_3(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Customer")
              .property( "state", [&](auto &prop) {
                auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto s = gdb->get_dictionary()->lookup_code(c);
                std::regex r(boost::get<std::string>(params[0]) + ".*");
                return std::regex_match(s, r); })
#else
              .nodes_where("Customer", "state", [&](auto &prop) {
                auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto s = gdb->get_dictionary()->lookup_code(c);
                std::regex r(boost::get<std::string>(params[0]) + ".*");
                return std::regex_match(s, r); })
#endif
              .from_relationships(":hasPlaced")
              .to_node("Order")
              .property("entry_d", [&](auto &prop) {
                return (*(reinterpret_cast<const ptime *>(prop.value_))) <
                        boost::get<ptime>(params[1]); })
              .from_relationships(":contains")
              .to_node("OrderLine")
              .property("delivery_d", [&](auto &prop) {
                return (*(reinterpret_cast<const ptime *>(prop.value_))) >
                        boost::get<ptime>(params[1]); })
              .project({PExpr_(2, pj::uint64_property(res, "id")),
                        PExpr_(4, pj::double_property(res, "amount")),
                        PExpr_(2, pj::ptime_property(res, "entry_d")) })
              .groupby({0, 2}, {{"sum", 1}})
              .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                if (boost::get<double>(q1[2]) == boost::get<double>(q2[2]))
                  return boost::get<ptime>(q1[1]) < boost::get<ptime>(q2[1]);
                return boost::get<double>(q1[2]) > boost::get<double>(q2[2]); })
              .collect(rs);
    q.start();
    rs.wait();
}

/* ------------------------------------------------------------------------ */

void gtpch_query_4(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Nation")
              .property( "name", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto sqtr = boost::get<ptime>(params[0]);
                auto eqtr = sqtr + hours(24 * 30 * 3);
                return sqtr <= dt && dt < eqtr; })
#else
              .nodes_where("Order", "entry_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto sqtr = boost::get<ptime>(params[0]);
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

void gtpch_query_5(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("Region")
              .property( "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
                return a == b; })
#else
              .nodes_where("Region", "name", [&](auto &prop) {
                auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
                auto b = gdb->get_dictionary()->lookup_string(boost::get<std::string>(params[0]));
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
                auto sdt = boost::get<ptime>(params[1]);
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

void gtpch_query_6(graph_db_ptr &gdb, result_set &rs, params_tuple &params) {

    auto q = query(gdb)
#ifdef RUN_PARALLEL
              .all_nodes()
              .has_label("OrderLine")
              .property( "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto sdt = boost::get<ptime>(params[0]);
                auto edt = sdt + hours(24 * 365);
                return sdt <= dt && dt < edt; })
#else
              .nodes_where("OrderLine", "delivery_d", [&](auto &prop) {
                auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
                auto sdt = boost::get<ptime>(params[0]);
                auto edt = sdt + hours(24 * 365);
                return sdt <= dt && dt < edt; })
#endif
              .property( "quantity", [&](auto &prop) {
                auto qtty = (*(reinterpret_cast<const int *>(prop.value_)));
                return qtty == boost::get<int>(params[1]); })
              .project({PExpr_(0, pj::double_property(res, "amount")) })
              .groupby({}, {{"sum", 0}})
              .collect(rs);
    q.start();
    rs.wait();
}