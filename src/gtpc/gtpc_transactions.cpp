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

static const uint64_t W_ID = 1;
static const auto WAREHOUSE_COUNT = 2;
static const auto ITEM_COUNT = 100000;
static const auto DISTRICT_PER_WAREHOUSE = 10;
static const auto CUSTOMER_PER_DISTRICT = 3000;
static const std::vector<std::string> C_LAST =
  {"BAR", "OUGHT", "ABLE", "PRI", "PRES", "ESE", "ANTI", "CALLY", "ATION", "EING"};

std::string gen_last_name(int num);
int gen_random_uniform_int(int min, int max);
int gen_random_nonuniform_int(int A, int x, int y, int C);

/* ------------------------------------------------------------------------ */

void gtpc_transaction_1(graph_db_ptr &gdb, result_set &rs) {

  /**
   *  retrieve warehouse
   */
  auto q1 = query(gdb)
  #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("Warehouse")
            .property("id", [&](auto &p) { return p.equal(W_ID); })
  #else
            .nodes_where("Warehouse", "id", [&](auto &p) { return p.equal(W_ID); })
  #endif
            .project({PExpr_(0, pj::double_property(res, "tax")) })
            .collect(rs);
  q1.start();

  /**
   *  retrieve district
   */
  uint64_t d_id = (W_ID - 1) * DISTRICT_PER_WAREHOUSE + (uint64_t)gen_random_uniform_int(1, 10);

  auto q2 = query(gdb)
  #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("District")
            .property("id", [&](auto &p) { return p.equal(d_id); })
  #else
            .nodes_where("District", "id", [&](auto &p) { return p.equal(d_id); })
  #endif
            .project({PExpr_(0, pj::double_property(res, "tax")),
                      PExpr_(0, pj::int_property(res, "next_o_id")) })
            .collect(rs);
  q2.start();

  /**
   *  update district
   */
  auto str_d_next_o_id = boost::get<std::string>(rs.data.back()[1]);
  auto d_next_o_id = std::stoi(str_d_next_o_id);
  d_next_o_id++;
  properties_t props_q3 = {{"next_o_id", d_next_o_id}};

  auto q3 = query(gdb)
  #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("District")
            .property("id", [&](auto &p) { return p.equal(d_id); })
  #else
            .nodes_where("District", "id", [&](auto &p) { return p.equal(d_id); })
  #endif
            .update(0, props_q3)
            // .collect(rs);
            .finish();
  q3.start();

  /**
   *  retrieve customer
   */
  auto c_min = (d_id - 1) * CUSTOMER_PER_DISTRICT + 1;
  auto c_max = d_id * CUSTOMER_PER_DISTRICT;
  uint64_t c_id = (uint64_t)gen_random_nonuniform_int(1023, c_min, c_max, gen_random_uniform_int(0, 1023));

  auto q4 = query(gdb)
  #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("Customer")
            .property("id", [&](auto &p) { return p.equal(c_id); })
  #else
            .nodes_where("Customer", "id", [&](auto &p) { return p.equal(c_id); })
  #endif
            .project({PExpr_(0, pj::double_property(res, "discount")),
                      PExpr_(0, pj::string_property(res, "last")),
                      PExpr_(0, pj::string_property(res, "credit")) })
            .collect(rs);
  q4.start();

  /**
   *  insert order
   */
  uint64_t o_id = WAREHOUSE_COUNT * DISTRICT_PER_WAREHOUSE * CUSTOMER_PER_DISTRICT +
                    (uint64_t)gen_random_uniform_int(1, 10);
  ptime o_entry_d = time_from_string(std::string("2012-10-30 00:00:00.000"));
  auto o_carrier_id = 0;
  auto o_ol_cnt = gen_random_uniform_int(5, 15);
  auto o_all_local = 1;
  auto o_new_order = 1;

  auto order = gdb->add_node("Order",
                            {{"id", o_id},
                            {"entry_d", o_entry_d},
                            {"carrier_id", o_carrier_id},
                            {"ol_cnt", o_ol_cnt},
                            {"all_local", o_all_local},
                            {"new_order", o_new_order}});

  node* c_node = nullptr;
  gdb->nodes_by_label("Customer", [&](node & n) {
    if (gdb->is_node_property(n, "id", [&](auto &p) { return p.equal(c_id); }))
      c_node = &n;
  });
  uint64_t customer = c_node->id();
  auto hasPlaced = gdb->add_relationship(customer, order, ":hasPlaced", {});

  for (auto ol_number = 1; ol_number <= o_ol_cnt; ol_number++) {
    auto ol_quantity = gen_random_uniform_int(1, 10);

    /**
     *  retrieve item
     */
    uint64_t i_id = gen_random_nonuniform_int(8191, 1, ITEM_COUNT, gen_random_uniform_int(0, 8191));

    auto q5 = query(gdb)
    #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("Item")
            .property("id", [&](auto &p) { return p.equal(i_id); })
    #else
            .nodes_where("Item", "id", [&](auto &p) { return p.equal(i_id); })
    #endif
            .project({PExpr_(0, pj::double_property(res, "price")),
                        PExpr_(0, pj::string_property(res, "name")),
                        PExpr_(0, pj::string_property(res, "data")) })
            .collect(rs);
    q5.start();

    auto str_i_price = boost::get<std::string>(rs.data.back()[0]);
    double i_price = std::stod(str_i_price);

    /**
     *  retrieve stock
     */
    uint64_t s_id = (W_ID - 1) * ITEM_COUNT + i_id;
    // auto x = gen_random_uniform_int(1, 100);
    // uint64_t wid = gen_random_uniform_int(1, WAREHOUSE_COUNT);
    // while (wid == W_ID)
    //   wid = gen_random_uniform_int(1, WAREHOUSE_COUNT);
    // if (x > 1)
    //   wid = W_ID;
    // uint64_t s_id = (wid - 1) * ITEM_COUNT + i_id;
    std::string s_dist_xx = "dist_" + ((d_id % 10) != 0 ? ("0" + std::to_string(d_id)) : "10");

    auto q6 = query(gdb)
    #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("Stock")
            .property("id", [&](auto &p) { return p.equal(s_id); })
    #else
            .nodes_where("Stock", "id", [&](auto &p) { return p.equal(s_id); })
    #endif
            .project({PExpr_(0, pj::int_property(res, "quantity")),
                        PExpr_(0, pj::string_property(res, s_dist_xx)),
                        PExpr_(0, pj::string_property(res, "data")),
                        PExpr_(0, pj::int_property(res, "ytd")),
                        PExpr_(0, pj::int_property(res, "order_cnt")),
                        PExpr_(0, pj::int_property(res, "quantity")),
                        PExpr_(0, pj::int_property(res, "remote_cnt")) })
            .collect(rs);
    q6.start();

    /**
     *  update stock
     */
    auto &res_vec = rs.data.back();
    auto str_s_ytd = boost::get<std::string>(res_vec[3]);
    auto s_ytd = std::stoi(str_s_ytd);
    auto str_s_order_cnt = boost::get<std::string>(res_vec[4]);
    auto s_order_cnt = std::stoi(str_s_order_cnt);
    auto str_s_quantity = boost::get<std::string>(res_vec[5]);
    auto s_quantity = std::stoi(str_s_quantity);
    auto str_s_remote_cnt = boost::get<std::string>(res_vec[6]);
    auto s_remote_cnt = std::stoi(str_s_remote_cnt);
    s_ytd += ol_quantity;
    s_order_cnt++;
    s_remote_cnt++;
    if ((s_quantity - ol_quantity) >= 10)
      s_quantity -= ol_quantity;
    else
      s_quantity = (s_quantity - ol_quantity) + 91;
    properties_t props_q7 = {{"ytd", s_ytd}, {"order_cnt", s_order_cnt},
                             {"quantity", s_quantity}, {"remote_cnt", s_remote_cnt}};

    auto q7 = query(gdb)
    #ifdef RUN_PARALLEL
          .all_nodes()
          .has_label("Stock")
          .property("id", [&](auto &p) { return p.equal(s_id); })
    #else
          .nodes_where("Stock", "id", [&](auto &p) { return p.equal(s_id); })
    #endif
          .update(0, props_q7)
          // .collect(rs);
          .finish();
    q7.start();


    /**
     *  insert orderline
     */
    uint64_t ol_id = (uint64_t)gen_random_uniform_int(2000000, 3000000);
    ptime ol_delivery_d = time_from_string(std::string("1970-01-01 00:00:00.000"));
    double ol_amount = ol_quantity * i_price;

    auto orderLine = gdb->add_node("OrderLine",
                                    {{"id", ol_id},
                                    {"number", ol_number},
                                    {"delivery_d", ol_delivery_d},
                                    {"quantity", ol_quantity},
                                    {"amount", ol_amount},
                                    {"dist_info", s_dist_xx}});

    auto contains = gdb->add_relationship(order, orderLine, ":contains", {});

    node* s_node = nullptr;
    gdb->nodes_by_label("Stock", [&](node & n) {
      if (gdb->is_node_property(n, "id", [&](auto &p) { return p.equal(s_id); }))
        s_node = &n;
    });
    uint64_t stock = s_node->id();
    auto hasStock = gdb->add_relationship(orderLine, stock, ":hasStock", {});
  }
}

/* ------------------------------------------------------------------------ */

void gtpc_transaction_2(graph_db_ptr &gdb, result_set &rs) {

  /**
   *  retrieve warehouse
   */
  auto q1 = query(gdb)
  #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("Warehouse")
            .property("id", [&](auto &p) { return p.equal(W_ID); })
  #else
              .nodes_where("Warehouse", "id", [&](auto &p) { return p.equal(W_ID); })
  #endif
            .project({PExpr_(0, pj::string_property(res, "name")),
                      PExpr_(0, pj::string_property(res, "street_1")),
                      PExpr_(0, pj::string_property(res, "street_2")),
                      PExpr_(0, pj::string_property(res, "city")),
                      PExpr_(0, pj::string_property(res, "state")),
                      PExpr_(0, pj::string_property(res, "zip")),
                      PExpr_(0, pj::double_property(res, "ytd")) })
            .collect(rs);
  q1.start();

  /**
   *  update warehouse
   */
  double h_amount = (double)gen_random_uniform_int(1, 5000);
  auto str_w_ytd = boost::get<std::string>(rs.data.back()[6]);
  double w_ytd = std::stod(str_w_ytd);
  w_ytd += h_amount;
  properties_t props_q2 = {{"ytd", str_w_ytd}};

  auto q2 = query(gdb)
  #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("Warehouse")
            .property("id", [&](auto &p) { return p.equal(W_ID); })
  #else
            .nodes_where("Warehouse", "id", [&](auto &p) { return p.equal(W_ID); })
  #endif
            .update(0, props_q2)
            // .collect(rs);
            .finish();
  q2.start();

  /**
   *  retrieve district
   */
  uint64_t d_id = (W_ID - 1) * DISTRICT_PER_WAREHOUSE + (uint64_t)gen_random_uniform_int(1, 10);

  auto q3 = query(gdb)
  #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("District")
            .property("id", [&](auto &p) { return p.equal(d_id); })
  #else
            .nodes_where("District", "id", [&](auto &p) { return p.equal(d_id); })
  #endif
            .project({PExpr_(0, pj::string_property(res, "name")),
                      PExpr_(0, pj::string_property(res, "street_1")),
                      PExpr_(0, pj::string_property(res, "street_2")),
                      PExpr_(0, pj::string_property(res, "city")),
                      PExpr_(0, pj::string_property(res, "state")),
                      PExpr_(0, pj::int_property(res, "zip")),
                      PExpr_(0, pj::double_property(res, "ytd")) })
            .collect(rs);
  q3.start();

  /**
   *  update district
   */
  auto str_d_ytd = boost::get<std::string>(rs.data.back()[6]);
  double d_ytd = std::stod(str_d_ytd);
  d_ytd += h_amount;
  properties_t props_q3 = {{"ytd", d_ytd}};

  auto q4 = query(gdb)
  #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("District")
            .property("id", [&](auto &p) { return p.equal(d_id); })
  #else
            .nodes_where("District", "id", [&](auto &p) { return p.equal(d_id); })
  #endif
            .update(0, props_q3)
            // .collect(rs);
            .finish();
  q4.start();

  /**
   *  retrieve and update customer
   */
  auto x = gen_random_uniform_int(1, 100);
  auto y = gen_random_uniform_int(1, 100);
  // uint64_t wid = gen_random_uniform_int(1, WAREHOUSE_COUNT);
  // while (wid == W_ID)
  //   wid = gen_random_uniform_int(1, WAREHOUSE_COUNT);
  // if (x <= 85)
  //   wid = W_ID;
  // uint64_t did = (wid - 1) * DISTRICT_PER_WAREHOUSE + (uint64_t)gen_random_uniform_int(1, 10);
  uint64_t did = (W_ID - 1) * DISTRICT_PER_WAREHOUSE + (uint64_t)gen_random_uniform_int(1, 10);
  auto c_min = (did - 1) * CUSTOMER_PER_DISTRICT + 1;
  auto c_max = did * CUSTOMER_PER_DISTRICT;
  uint64_t c_id = (uint64_t)gen_random_nonuniform_int(1023, c_min, c_max, gen_random_uniform_int(0, 1023));
  std::string c_credit = "";
  std::string c_data = "";

  if (y > 60) {
    auto q5 = query(gdb)
    #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("Customer")
            .property("id", [&](auto &p) { return p.equal(c_id); })
    #else
            .nodes_where("Customer", "id", [&](auto &p) { return p.equal(c_id); })
    #endif
            .project({PExpr_(0, pj::string_property(res, "first")),
                    PExpr_(0, pj::string_property(res, "middle")),
                    PExpr_(0, pj::string_property(res, "last")),
                    PExpr_(0, pj::string_property(res, "street_1")),
                    PExpr_(0, pj::string_property(res, "street_2")),
                    PExpr_(0, pj::string_property(res, "city")),
                    PExpr_(0, pj::string_property(res, "state")),
                    PExpr_(0, pj::int_property(res, "zip")),
                    PExpr_(0, pj::ptime_property(res, "since")),
                    PExpr_(0, pj::string_property(res, "credit")),
                    PExpr_(0, pj::double_property(res, "credit_lim")),
                    PExpr_(0, pj::double_property(res, "discount")),
                    PExpr_(0, pj::double_property(res, "balance")),
                    PExpr_(0, pj::double_property(res, "ytd_payment")),
                    PExpr_(0, pj::int_property(res, "payment_cnt")),
                    PExpr_(0, pj::string_property(res, "data")) })
            .collect(rs);
    q5.start();

    auto &res_vec = rs.data.back();
    auto str_c_balance = boost::get<std::string>(res_vec[12]);
    auto c_balance = std::stod(str_c_balance);
    c_balance += h_amount;
    auto str_c_ytd_payment = boost::get<std::string>(res_vec[13]);
    auto c_ytd_payment = std::stod(str_c_ytd_payment);
    c_ytd_payment += h_amount;
    auto str_c_payment_cnt = boost::get<std::string>(res_vec[14]);
    auto c_payment_cnt = std::stoi(str_c_payment_cnt);
    c_payment_cnt++;
    c_data = boost::get<std::string>(res_vec[15]);
    c_credit = boost::get<std::string>(res_vec[9]);
    properties_t props_q6 = {{"balance", c_balance},
                              {"ytd_payment", c_ytd_payment},
                              {"payment_cnt", c_payment_cnt}};

    auto q6 = query(gdb)
    #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("Customer")
            .property("id", [&](auto &p) { return p.equal(c_id); })
    #else
            .nodes_where("Customer", "id", [&](auto &p) { return p.equal(c_id); })
    #endif
            .update(0, props_q6)
            // .collect(rs);
            .finish();
    q6.start();
  }
  else {
    auto c_last = gen_last_name(gen_random_uniform_int(0, 999));
    auto q7 = query(gdb)
    #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("District")
            .property("id", [&](auto &p) { return p.equal(d_id); })
    #else
            .nodes_where("District", "id", [&](auto &p) { return p.equal(d_id); })
    #endif
            .from_relationships(":serves")
            .to_node("Customer")
            .property( "last", [&](auto &prop) {
              auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
              auto b = gdb->get_dictionary()->lookup_string(c_last);
              return a == b; })
            .project({PExpr_(2, pj::uint64_property(res, "id")),
                    PExpr_(2, pj::string_property(res, "first")),
                    PExpr_(2, pj::string_property(res, "middle")),
                    PExpr_(2, pj::string_property(res, "street_1")),
                    PExpr_(2, pj::string_property(res, "street_2")),
                    PExpr_(2, pj::string_property(res, "city")),
                    PExpr_(2, pj::string_property(res, "state")),
                    PExpr_(2, pj::int_property(res, "zip")),
                    PExpr_(2, pj::ptime_property(res, "since")),
                    PExpr_(2, pj::string_property(res, "credit")),
                    PExpr_(2, pj::double_property(res, "credit_lim")),
                    PExpr_(2, pj::double_property(res, "discount")),
                    PExpr_(2, pj::double_property(res, "balance")),
                    PExpr_(2, pj::double_property(res, "ytd_payment")),
                    PExpr_(2, pj::int_property(res, "payment_cnt")),
                    PExpr_(2, pj::string_property(res, "data")) })
            .limit(1)
            .collect(rs);
    q7.start();

    auto &res_vec = rs.data.back();
    auto str_c_id = boost::get<std::string>(res_vec[0]);
    c_id = (uint64_t)std::stoi(str_c_id);
    auto str_c_balance = boost::get<std::string>(res_vec[12]);
    auto c_balance = std::stod(str_c_balance);
    c_balance += h_amount;
    auto str_c_ytd_payment = boost::get<std::string>(res_vec[13]);
    auto c_ytd_payment = std::stod(str_c_ytd_payment);
    c_ytd_payment += h_amount;
    auto str_c_payment_cnt = boost::get<std::string>(res_vec[14]);
    auto c_payment_cnt = std::stoi(str_c_payment_cnt);
    c_payment_cnt++;
    c_data = boost::get<std::string>(res_vec[15]);
    c_credit = boost::get<std::string>(res_vec[9]);
    properties_t props_q8 = {{"balance", c_balance},
                              {"ytd_payment", c_ytd_payment},
                              {"payment_cnt", c_payment_cnt}};

    auto q8 = query(gdb)
    #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("Customer")
            .property("id", [&](auto &p) { return p.equal(c_id); })
    #else
            .nodes_where("Customer", "id", [&](auto &p) { return p.equal(c_id); })
    #endif
            .update(0, props_q8)
            // .collect(rs);
            .finish();
    q8.start();

  }

  if (c_credit == "BC") {
    c_data = c_data + std::to_string(c_id) + std::to_string(h_amount);
    properties_t props_q9 = {{"data", c_data}};

    auto q9 = query(gdb)
    #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("Customer")
            .property("id", [&](auto &p) { return p.equal(c_id); })
    #else
            .nodes_where("Customer", "id", [&](auto &p) { return p.equal(c_id); })
    #endif
            .update(0, props_q9)
            // .collect(rs);
            .finish();
    q9.start();
  }
}

/* ------------------------------------------------------------------------ */

void gtpc_transaction_3(graph_db_ptr &gdb, result_set &rs) {

  /**
   *  retrieve customer
   */
  auto x = gen_random_uniform_int(1, 100);
  auto y = gen_random_uniform_int(1, 100);
  // uint64_t wid = gen_random_uniform_int(1, WAREHOUSE_COUNT);
  // while (wid == W_ID)
  //   wid = gen_random_uniform_int(1, WAREHOUSE_COUNT);
  // if (x <= 85)
  //   wid = W_ID;
  // uint64_t did = (wid - 1) * DISTRICT_PER_WAREHOUSE + (uint64_t)gen_random_uniform_int(1, 10);
  uint64_t did = (W_ID - 1) * DISTRICT_PER_WAREHOUSE + (uint64_t)gen_random_uniform_int(1, 10);
  auto c_min = (did - 1) * CUSTOMER_PER_DISTRICT + 1;
  auto c_max = did * CUSTOMER_PER_DISTRICT;
  uint64_t c_id = (uint64_t)gen_random_nonuniform_int(1023, c_min, c_max, gen_random_uniform_int(0, 1023));
  std::string c_credit = "";
  std::string c_data = "";

  if (y > 60) {
    auto q1 = query(gdb)
    #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("Customer")
            .property("id", [&](auto &p) { return p.equal(c_id); })
    #else
            .nodes_where("Customer", "id", [&](auto &p) { return p.equal(c_id); })
    #endif
          .project({PExpr_(0, pj::double_property(res, "balance")),
                    PExpr_(0, pj::string_property(res, "first")),
                    PExpr_(0, pj::string_property(res, "middle")),
                    PExpr_(0, pj::string_property(res, "last")) })
            .collect(rs);
    q1.start();
  }
  else {
    auto c_last = gen_last_name(gen_random_uniform_int(0, 999));
    auto q2 = query(gdb)
    #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("District")
            .property("id", [&](auto &p) { return p.equal(did); })
    #else
            .nodes_where("District", "id", [&](auto &p) { return p.equal(did); })
    #endif
            .from_relationships(":serves")
            .to_node("Customer")
            .property( "last", [&](auto &prop) {
              auto a = *(reinterpret_cast<const dcode_t *>(prop.value_));
              auto b = gdb->get_dictionary()->lookup_string(c_last);
              return a == b; })
            .project({PExpr_(2, pj::double_property(res, "balance")),
                      PExpr_(2, pj::string_property(res, "first")),
                      PExpr_(2, pj::string_property(res, "middle")),
                      PExpr_(2, pj::string_property(res, "last")),
                      PExpr_(2, pj::uint64_property(res, "id")) })
            .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
              return boost::get<std::string>(q1[1]) < boost::get<std::string>(q2[1]); })
            .limit(1)
            .collect(rs);
    q2.start();

    auto str_c_id = boost::get<std::string>(rs.data.back()[4]);
    c_id = (uint64_t)std::stoi(str_c_id);
  }

  /**
   *  retrieve order
   */
  auto q3 = query(gdb)
  #ifdef RUN_PARALLEL
          .all_nodes()
          .has_label("Customer")
          .property("id", [&](auto &p) { return p.equal(c_id); })
  #else
          .nodes_where("Customer", "id", [&](auto &p) { return p.equal(c_id); })
  #endif
          .from_relationships(":hasPlaced")
          .to_node("Order")
          .project({PExpr_(2, pj::uint64_property(res, "id")),
                    PExpr_(2, pj::ptime_property(res, "entry_d")),
                    PExpr_(2, pj::int_property(res, "carrier_id")) })
          .collect(rs);
  q3.start();

  auto str_o_id = boost::get<std::string>(rs.data.back()[0]);
  uint64_t o_id = (uint64_t)std::stoi(str_o_id);

  /**
   *  retrieve orderline
   */
  auto q4 = query(gdb)
  #ifdef RUN_PARALLEL
          .all_nodes()
          .has_label("Order")
          .property("id", [&](auto &p) { return p.equal(o_id); })
  #else
          .nodes_where("Order", "id", [&](auto &p) { return p.equal(o_id); })
  #endif
          .from_relationships(":contains")
          .to_node("OrderLine")
          .project({PExpr_(2, pj::uint64_property(res, "id")),
                    PExpr_(2, pj::int_property(res, "quantity")),
                    PExpr_(2, pj::double_property(res, "amount")),
                    PExpr_(2, pj::ptime_property(res, "delivery_d")) })
          .collect(rs);
  q4.start();
}

/* ------------------------------------------------------------------------ */

void gtpc_transaction_4(graph_db_ptr &gdb, result_set &rs) {

  uint64_t d_min = (W_ID - 1) * 10 + 1;
  uint64_t d_max = (W_ID - 1) * 10 + 10;

  for (auto d_id = d_min; d_id <= d_max; d_id++) {
    /**
     *  retrieve order
     */
    auto q1 = query(gdb)
    #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("District")
            .property("id", [&](auto &p) { return p.equal(d_id); })
    #else
            .nodes_where("District", "id", [&](auto &p) { return p.equal(d_id); })
    #endif
            .from_relationships(":serves")
            .to_node("Customer")
            .from_relationships(":hasPlaced")
            .to_node("Order")
            .project({PExpr_(4, pj::uint64_property(res, "id")),
                      PExpr_(2, pj::uint64_property(res, "id")),
                      PExpr_(4, pj::int_property(res, "new_order")),
                      PExpr_(4, pj::int_property(res, "carrier_id")) })
            .where_qr_tuple([&](const qr_tuple &v) { return boost::get<int>(v[2]) > 0; })
            .orderby([&](const qr_tuple &q1, const qr_tuple &q2) {
                return boost::get<uint64_t>(q1[0]) < boost::get<uint64_t>(q2[0]); })
            .limit(1)
            .collect(rs);
    q1.start();

    /**
     *  update order
     */
    auto &res_vec = rs.data.back();
    auto str_o_id = boost::get<std::string>(res_vec[0]);
    uint64_t o_id = std::stoi(str_o_id);
    auto str_c_id = boost::get<std::string>(res_vec[1]);
    uint64_t c_id = std::stoi(str_c_id);
    auto o_new_order = 0;
    auto o_carrier_id = gen_random_uniform_int(1, 10);
    properties_t props_q2 = {{"new_order", o_new_order}, {"carrier_id", o_carrier_id}};

    auto q2 = query(gdb)
    #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("Order")
            .property("id", [&](auto &p) { return p.equal(o_id); })
    #else
            .nodes_where("Order", "id", [&](auto &p) { return p.equal(o_id); })
    #endif
            .update(0, props_q2)
            // .collect(rs);
            .finish();
    q2.start();

    /**
     *  update orderline
     */
    ptime ol_delivery_d = time_from_string(std::string("2012-10-30 00:00:00.000"));
    properties_t props_q3 = {{"delivery_d", ol_delivery_d}};

    auto q3 = query(gdb)
    #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("Order")
            .property("id", [&](auto &p) { return p.equal(o_id); })
    #else
            .nodes_where("Order", "id", [&](auto &p) { return p.equal(o_id); })
    #endif
            .from_relationships(":contains")
            .to_node("OrderLine")
            .update(2, props_q3)
            // .collect(rs);
            .finish();
        q3.start();

    /**
     *  retrieve orderline
     */
    auto q4 = query(gdb)
    #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("Order")
            .property("id", [&](auto &p) { return p.equal(o_id); })
    #else
            .nodes_where("Order", "id", [&](auto &p) { return p.equal(o_id); })
    #endif
            .from_relationships(":contains")
            .to_node("OrderLine")
            .project({PExpr_(2, pj::double_property(res, "amount")) })
            .groupby({}, {{"sum", 0}})
            .collect(rs);
        q4.start();

    /**
     *  update customer
     */
    auto str_sum_ol_amount = boost::get<std::string>(rs.data.back()[0]);
    double sum_ol_amount = std::stod(str_sum_ol_amount);
    auto q5a = query(gdb)
    #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("Customer")
            .property("id", [&](auto &p) { return p.equal(c_id); })
    #else
            .nodes_where("Customer", "id", [&](auto &p) { return p.equal(c_id); })
    #endif
            .project({PExpr_(0, pj::double_property(res, "balance")),
                      PExpr_(0, pj::int_property(res, "delivery_cnt")) })
            .collect(rs);
    q5a.start();

    auto str_c_balance = boost::get<std::string>(rs.data.back()[0]);
    double c_balance = std::stod(str_c_balance);
    c_balance += sum_ol_amount;
    auto str_c_delivery_cnt = boost::get<std::string>(rs.data.back()[1]);
    auto c_delivery_cnt = std::stoi(str_c_delivery_cnt);
    c_delivery_cnt++;
    properties_t props_q5b = {{"balance", c_balance}, {"delivery_cnt", c_delivery_cnt}};

    auto q5b = query(gdb)
    #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("Customer")
            .property("id", [&](auto &p) { return p.equal(c_id); })
    #else
            .nodes_where("Customer", "id", [&](auto &p) { return p.equal(c_id); })
    #endif
            .update(0, props_q5b)
            // .collect(rs);
            .finish();
    q5b.start();
  }
}

/* ------------------------------------------------------------------------ */

void gtpc_transaction_5(graph_db_ptr &gdb, result_set &rs) {

  /**
   *  retrieve district
   */
  auto threshold = gen_random_uniform_int(10, 20);
  uint64_t d_id = (W_ID - 1) * DISTRICT_PER_WAREHOUSE + (uint64_t)gen_random_uniform_int(1, 10);
  auto q1 = query(gdb)
  #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("District")
            .property("id", [&](auto &p) { return p.equal(d_id); })
  #else
            .nodes_where("District", "id", [&](auto &p) { return p.equal(d_id); })
  #endif
            .project({PExpr_(0, pj::int_property(res, "next_o_id")) })
            .collect(rs);
  q1.start();

  /**
   *  retrieve district
   */
  auto str_d_next_o_id = boost::get<std::string>(rs.data.back()[0]);
  uint64_t d_next_o_id = (uint64_t)std::stoi(str_d_next_o_id);
  auto q2 = query(gdb)
  #ifdef RUN_PARALLEL
            .all_nodes()
            .has_label("District")
            .property("id", [&](auto &p) { return p.equal(d_id); })
  #else
            .nodes_where("District", "id", [&](auto &p) { return p.equal(d_id); })
  #endif
            .from_relationships(":serves")
            .to_node("Customer")
            .from_relationships(":hasPlaced")
            .to_node("Order")
            .property( "id", [&](auto &prop) {
              auto oid = *(reinterpret_cast<const uint64_t *>(prop.value_));
              return (d_next_o_id - 200) <= oid && oid < d_next_o_id; })
            .from_relationships(":contains")
            .to_node("OrderLine")
            .from_relationships(":hasStock")
            .to_node("Stock")
            .property( "quantity", [&](auto &prop) {
              auto quantity = *(reinterpret_cast<const int *>(prop.value_));
              return quantity < threshold; })
            .to_relationships(":hasStock")
            .from_node("Item")
            .project({PExpr_(10, pj::uint64_property(res, "id")) })
            .distinct()
            .count()
            .collect(rs);
  q2.start();
}

/* ------------------------------------------------------------------------ */

int gen_random_uniform_int(int min, int max) {
	return (rand() % (max - min + 1)) + min;
}

int gen_random_nonuniform_int(int A, int x, int y, int C) {
	return ((((gen_random_uniform_int(0, A) | gen_random_uniform_int(x, y)) + C) % ( y - x + 1)) + x);
}

std::string gen_last_name(int num) {
    return C_LAST[num / 100] + C_LAST[(num / 10) % 10] + C_LAST[num % 10];
}