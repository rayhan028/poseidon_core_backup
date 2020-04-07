/*
 * Copyright (C) 2019-2020 DBIS Group - TU Ilmenau, All Rights Reserved.
 *
 * This file is part of the Poseidon package.
 *
 * Poseidon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Poseidon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Poseidon. If not, see <http://www.gnu.org/licenses/>.
 */

#include <boost/algorithm/string.hpp>

#include "graph_db.hpp"
#include "chunked_vec.hpp"
#include "parser.hpp"
#include "spdlog/spdlog.h"
#include "thread_pool.hpp"
#include <iostream>

#ifdef USE_PMDK
namespace nvm = pmem::obj;
#endif

node::id_t graph_db::import_node(const std::string &label,
                                 const properties_t &props) {
  auto type_code = dict_->insert(label);
  auto node_id = nodes_->append(node(type_code), 0);

  // we need the node object not only the id
  auto &n = nodes_->get(node_id);

  // save properties
  if (!props.empty()) {
    property_set::id_t pid =
        properties_->append_node_properties(node_id, props, dict_);
    n.property_list = pid;
  }

  return node_id;
}

node::id_t graph_db::import_typed_node(const std::string &label, 
                              const std::vector<dcode_t> &keys,
                              const std::vector<p_item::p_typecode>& typelist, 
                              const std::vector<boost::any>& values) {
  auto type_code = dict_->insert(label);
  auto node_id = nodes_->append(node(type_code), 0);
                            
  // we need the node object not only the id
  auto &n = nodes_->get(node_id);

  // save properties
  if (!keys.empty()) {
    property_set::id_t pid =
        properties_->append_typed_node_properties(node_id, keys, typelist, values);
    n.property_list = pid;
  }

  return node_id;
}

relationship::id_t graph_db::import_relationship(node::id_t from_id,
                                                 node::id_t to_id,
                                                 const std::string &label,
                                                 const properties_t &props) {
  auto &from_node = nodes_->get(from_id);
  auto &to_node = nodes_->get(to_id);
  auto type_code = dict_->insert(label);
  auto rid = rships_->append(relationship(type_code, from_id, to_id), 0);

  auto &r = rships_->get(rid);

  // save properties
  if (!props.empty()) {
    property_set::id_t pid =
        properties_->append_relationship_properties(rid, props, dict_);
    r.property_list = pid;
  }
  // update the list of relationships for each of both nodes
  if (from_node.from_rship_list == UNKNOWN)
    from_node.from_rship_list = rid;
  else {
    r.next_src_rship = from_node.from_rship_list;
    from_node.from_rship_list = rid;
  }

  if (to_node.to_rship_list == UNKNOWN)
    to_node.to_rship_list = rid;
  else {
    r.next_dest_rship = to_node.to_rship_list;
    to_node.to_rship_list = rid;
  }
  return rid;
}

relationship::id_t graph_db::import_typed_relationship(node::id_t from_id,
                                         node::id_t to_id,
                                         const std::string &label, 
                                         const std::vector<dcode_t> &keys,
                                         const std::vector<p_item::p_typecode>& typelist, 
                                         const std::vector<boost::any>& values) {
  auto &from_node = nodes_->get(from_id);
  auto &to_node = nodes_->get(to_id);
  auto type_code = dict_->insert(label);
  auto rid = rships_->append(relationship(type_code, from_id, to_id), 0);

  auto &r = rships_->get(rid);

  // save properties
  if (!keys.empty()) {
    property_set::id_t pid =
        properties_->append_typed_node_properties(rid, keys, typelist, values);
    r.property_list = pid;
  }

  // update the list of relationships for each of both nodes
  if (from_node.from_rship_list == UNKNOWN)
    from_node.from_rship_list = rid;
  else {
    r.next_src_rship = from_node.from_rship_list;
    from_node.from_rship_list = rid;
  }

  if (to_node.to_rship_list == UNKNOWN)
    to_node.to_rship_list = rid;
  else {
    r.next_dest_rship = to_node.to_rship_list;
    to_node.to_rship_list = rid;
  }
  return rid;

}

std::size_t graph_db::import_nodes_from_csv(const std::string &label,
                                            const std::string &filename,
                                            char delim, mapping_t &m) {
  using namespace aria::csv;

  std::ifstream f(filename);
  if (!f.is_open())
    return 0;

  CsvParser parser = CsvParser(f).delimiter(delim);
  std::size_t num = 0;

  std::vector<std::string> columns; // name of all fields
  int id_column = -1;               // field no of :ID

  for (auto &row : parser) {
    if (num == 0) {
      // process the header
      std::size_t i = 0;
      for (auto &field : row) {
        //auto pos = field.find(":ID"); // neo4j
        auto pos = field.find("id");
        if (pos != std::string::npos) {
          // <name>:ID is a special field // neo4j
          id_column = i;
          //columns.push_back(field.substr(0, pos)); // neo4j
           columns.push_back(field);
        } else
          columns.push_back(field);
        i++;
      }
      assert(id_column >= 0);
    } else {
      properties_t props;
      auto i = 0;
      std::string id_label;
      for (auto &field : row) {
        if (i == id_column)
          id_label = field;

        auto &col = columns[i++];
        //if (!col.empty() && !field.empty()) {
        if (!col.empty() && !(field.empty() && col != "content")) {
          if (col == "id")
            props.insert({col, (uint64_t)std::stoll(field)});
          else
            props.insert({col, field});
        }
      }
      auto id = import_node(label, props);
      auto id_label_s = id_label + "_" + label;
      m.insert({id_label_s, id});
      // std::cout << "mapping: " << id_label << " -> " << id << std::endl;
    }
    num++;
  }

  return num;
}

std::size_t graph_db::import_relationships_from_csv(const std::string &filename,
                                                    char delim,
                                                    const mapping_t &m) {
  using namespace aria::csv;

  std::ifstream f(filename);
  if (!f.is_open())
    return 0;

  CsvParser parser = CsvParser(f).delimiter(delim);
  std::size_t num = 0;

  std::vector<std::string> fp;
  boost::split(fp, filename, boost::is_any_of("/"));
  assert(fp.back().find(".csv") != std::string::npos);
  std::vector<std::string> fn;
  boost::split(fn, fp.back(), boost::is_any_of("_"));
  auto label = ":" + fn[1];
  auto src_node = fn[0];
  auto des_node = fn[2];

  std::vector<std::string> columns;
  //int start_col = -1, end_col = -1, type_col = -1; // neo4j
  int start_col = 0, end_col = 1;

  for (auto &row : parser) {
    if (num == 0) {
      auto i = 0;
      // process header
      for (auto &field : row) {
        columns.push_back(field);
        /*if (field == ":START_ID") // neo4j
          start_col = i;
        else if (field == ":END_ID")
          end_col = i;
        else if (field == ":TYPE")
          type_col = i;*/
        i++;
      }
      /*assert(start_col >= 0); // neo4j
      assert(end_col >= 0);
      assert(type_col >= 0);*/
    } else {
      if (src_node[0] >= 'a' && src_node[0] <= 'z')
        src_node[0] -= 32;
      auto src_id_s = row[start_col] + "_" + src_node;
      mapping_t::const_iterator it = m.find(src_id_s);
      if (it == m.end())
        continue;
      node::id_t from_node = it->second;

      if (des_node[0] >= 'a' && des_node[0] <= 'z')
        des_node[0] -= 32;
      auto des_id_s = row[end_col] + "_" + des_node;
      it = m.find(des_id_s);
      if (it == m.end())
        continue;
      node::id_t to_node = it->second;

      //auto &label = row[type_col]; // neo4j

      properties_t props;
      auto i = 0;
      for (auto &field : row) {
        //if (i != start_col && i != end_col && i != type_col) {  // neo4j
        if (i != start_col && i != end_col) {
          auto &col = columns[i];
          if (!field.empty())
            props.insert({col, field});
        }
        i++;
      }
      import_relationship(from_node, to_node, label, props);
      // std::cout << "add rship: " << row[start_col] << "(" << from_node <<
      // ")->"
      //    << row[end_col] << "(" << to_node << ")" << std::endl;
    }
    num++;
  }

  return num;
}
