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
#include "vec.hpp"
#include "parser.hpp"
#include "spdlog/spdlog.h"
#include "thread_pool.hpp"
#include <iostream>

#ifdef USE_PMDK
namespace nvm = pmem::obj;
#endif

std::any string_to_any(p_item::p_typecode tc, const std::string& s, dict_ptr &dict) {
  switch (tc) {
    case p_item::p_dcode: return std::any(dict->insert(s));
    case p_item::p_int: return std::any((int)std::stoi(s));
    case p_item::p_uint64: return std::any((uint64_t)std::stoull(s));
    case p_item::p_double: return std::any((double)std::stod(s));
    case p_item::p_date:
    {
        boost::gregorian::date dt = boost::gregorian::from_simple_string(s);
        return std::any(boost::posix_time::ptime(dt, boost::posix_time::seconds(0)));
    }
    case p_item::p_ptime: 
    {
        std::string s2 = s;
        s2[s.find("T")] = ' ';
        auto dts = s2.substr(0, s2.find("+"));
        return std::any(boost::posix_time::time_from_string(dts));
    }
      default: return std::any();
  }
}

std::pair<p_item::p_typecode, std::any> 
infer_datatype(const std::string& s, dict_ptr &dict) {
  if (is_quoted_string(s))
    return std::make_pair(p_item::p_dcode, std::any(dict->insert(s)));
  else if (is_int(s)) {
    int ival = 0;
    try {
      ival = std::stoi(s);
    }
    catch (std::out_of_range& exc) {
      try {
        return std::make_pair(p_item::p_uint64, std::any((u_int64_t)std::stoull(s)));
      } catch (std::exception& exc) {
        spdlog::info("ERROR: cannot parse '{}': {}", s, exc.what());
      }
    }
    return std::make_pair(p_item::p_int, std::any(ival));
  }
  else if (is_float(s))
    return std::make_pair(p_item::p_double, std::any((double)std::stod(s)));
  else if (is_date(s)) {
    boost::gregorian::date dt = boost::gregorian::from_simple_string(s);
    return std::make_pair(p_item::p_date, std::any(boost::posix_time::ptime(dt, boost::posix_time::seconds(0))));
  }
  else if (is_dtime(s)) {
    std::string s2 = s;
    s2[s2.find("T")] = ' ';
    auto dts = s2.substr(0, s2.find("+"));
    return std::make_pair(p_item::p_ptime, std::any(boost::posix_time::time_from_string(dts)));
  }
  return std::make_pair(p_item::p_dcode, std::any(dict->insert(s)));
}

p_item::p_typecode
get_datatype(const std::string& s) {
    if (is_quoted_string(s))  return p_item::p_dcode;
    else if (is_uint64(s))      return p_item::p_uint64;
    else if (is_int(s))       return p_item::p_int;
    else if (is_float(s))     return p_item::p_double;
    else if (is_date(s))      return p_item::p_date;
    else if (is_dtime(s))     return p_item::p_ptime;
    return p_item::p_dcode;
}



node::id_t graph_db::import_node(const std::string &label,
                                 const properties_t &props) {
  auto type_code = dict_->insert(label);
  auto node_id = nodes_->append(node(type_code), 0);

  // we need the node object not only the id
  auto &n = nodes_->get(node_id);

  // save properties
  if (!props.empty()) {
    property_set::id_t pid =
        node_properties_->append_properties(node_id, props, dict_);
    n.property_list = pid;
  }

  return node_id;
}

node::id_t graph_db::import_typed_node(dcode_t label, 
                              const std::vector<dcode_t> &keys,
                              const std::vector<p_item::p_typecode>& typelist, 
                              const std::vector<std::any>& values) {
  auto node_id = nodes_->append(node(label), 0);
                            
  // we need the node object not only the id
  auto &n = nodes_->get(node_id);

  // save properties
  if (!keys.empty()) {
    property_set::id_t pid =
        node_properties_->append_typed_properties(node_id, keys, typelist, values);
    n.property_list = pid;
  }

  return node_id;
}

node::id_t graph_db::import_typed_node(dcode_t label,
                              const std::vector<dcode_t> &keys,
                              const std::vector<p_item::p_typecode>& typelist,
				const std::vector<std::string>& values, dict_ptr &dict) {
  auto node_id = nodes_->append(node(label), 0);

  // we need the node object not only the id
  auto &n = nodes_->get(node_id);

  // save properties
  if (!keys.empty()) {
    property_set::id_t pid =
        node_properties_->append_typed_properties(node_id, keys, typelist, values, dict);
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
        rship_properties_->append_properties(rid, props, dict_);
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
                                         dcode_t label, 
                                         const std::vector<dcode_t> &keys,
                                         const std::vector<p_item::p_typecode>& typelist, 
                                         const std::vector<std::any>& values) {
  auto &from_node = nodes_->get(from_id);
  auto &to_node = nodes_->get(to_id);
  auto rid = rships_->append(relationship(label, from_id, to_id), 0);

  auto &r = rships_->get(rid);

  // save properties
  if (!keys.empty()) {
    property_set::id_t pid =
        rship_properties_->append_typed_properties(rid, keys, typelist, values);
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
                                         dcode_t label,
                                         const std::vector<dcode_t> &keys,
                                         const std::vector<p_item::p_typecode>& typelist,
					  const std::vector<std::string>& values,dict_ptr &dict) {
  // std::cout << "<" << std::flush;
  auto &from_node = nodes_->get(from_id);
  auto &to_node = nodes_->get(to_id);
  // std::cout << "x" << std::flush;
  auto rid = rships_->append(relationship(label, from_id, to_id), 0);

  auto &r = rships_->get(rid);

  // save properties
  if (!keys.empty()) {
    // std::cout << "o" << std::flush;
    property_set::id_t pid =
        rship_properties_->append_typed_properties(rid, keys, typelist, values, dict);
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
  // std::cout << ">" << std::flush;

  return rid;

}


std::size_t graph_db::import_nodes_from_csv(const std::string &label,
                                            const std::string &filename,
                                            char delim, std::optional<mapping_t> &m, std::mutex *mtx) {
  using namespace aria::csv;

  std::ifstream f(filename);
  if (!f.is_open())
    return 0;

  CsvParser parser = CsvParser(f).delimiter(delim);
  std::size_t num = 0;

  std::vector<std::string> columns; // name of all fields

  for (auto &row : parser) {
    if (num == 0) {
      // process the header
      for (auto &field : row) 
        columns.push_back(field);

      num++;
    } 
    else {
      properties_t props;
      auto i = 0;
      std::string id_label;
      for (auto &field : row) {
        auto &col = columns[i++];
        if (!col.empty() && !field.empty()) {
            props.insert({col, field});
        }
      }
      try {
        auto id = import_node(label, props);
        auto id_label_s = id_label + "_" + label;
        if (m.has_value())
          m.value().insert({id_label_s, id});
        num++;
      } catch (std::exception& exc) {
        spdlog::warn("ERROR in import_nodes_from_csv during import of '{}': {} in line: {}", label, exc.what(), num);
      }
    }

  }
  return num > 0 ? num-1 : 0;
}

std::size_t graph_db::import_typed_nodes_from_csv(const std::string &label,
                                            const std::string &filename,
                                            char delim, std::optional<mapping_t> &m, typespec_t &ty, std::mutex *mtx) {
  using namespace aria::csv;

  std::ifstream f(filename);
  if (!f.is_open()) {
    spdlog::warn("cannot find node file '{}'", filename);
    return 0;
  }

  std::string id_label;
  auto label_code = dict_->insert(label);
  CsvParser parser = CsvParser(f).delimiter(delim);
  std::size_t num = 0;

  std::vector<std::string> columns; // names of all fields

  std::vector<dcode_t> prop_names;
  std::vector<p_item::p_typecode> prop_types; 
  std::vector<bool> inferred;
  node::id_t id;

  for (auto &row : parser) {
    if (num == 0) {
      // process the header
      for (auto &field : row)
        columns.push_back(field);

      prop_names.resize(columns.size());
      prop_types.resize(columns.size());
      inferred.resize(columns.size(), false);
      for (auto j = 0u; j < columns.size(); j++)
        prop_names[j] = dict_->insert(columns[j]);
      
    }
    else if (num == 1) {
      // process the first row: infer the data types or consult the typespec file
      // Not all data types are inferred from first row
      auto i = 0;
      for (auto &field : row) {
        if (const auto& col {columns[i]}; !col.empty() && !field.empty()) {
          auto qcol = label + "." + columns[i];
          auto it = ty.find(qcol);
          prop_types[i] = it != ty.end() ? it->second : get_datatype(field);
          inferred[i] = true;
          if (i == 0) id_label = field;
        }  
        else {
          // the field is empty, let's assume a string value
            prop_types[i] = p_item::p_dcode;
        }    
        i++;
      }
      if (mtx != nullptr) mtx->lock();
      
      try {
        id = import_typed_node(label_code, prop_names, prop_types, row, dict_);
      } catch (std::exception& exc) {
       spdlog::warn("ERROR in import_typed_nodes_from_csv during import of '{}': {} in line: {}", label, exc.what(), num);
        return num - 1;
      }
      // fill mapping table
      if (m.has_value()) {
        auto id_label_s = id_label + "_" + label;
        spdlog::debug("insert mapping: {}", id_label_s);
        m.value().insert({id_label_s, id});
      }
      
      if (mtx != nullptr) mtx->unlock();
    } 
    else {
      // any other row
      std::string id_label;
      auto i = 0;
      for (auto &field : row) {
        if (const auto& col {columns[i]}; !col.empty() && !field.empty()) {
          if (!inferred[i]) { 
            // columns whose datatypes we have not yet inferred
        	  prop_types[i] = get_datatype(field);
            inferred[i] = true;
          }
          if (i == 0) id_label = field;
        }   
        i++;
      }

      if (mtx != nullptr) mtx->lock();
      try {
        id = import_typed_node(label_code, prop_names, prop_types, row, dict_);
      } catch (std::exception& exc) {
        spdlog::warn("ERROR in import_typed_nodes_from_csv during import of '{}': {} in line: {}", label, exc.what(), num);
        return num - 1;
      }
      auto id_label_s = id_label + "_" + label;
      if (m.has_value())
        m.value().insert({id_label_s, id});

      if (mtx != nullptr) mtx->unlock();
    }
    num++;
  }
  return num - 1;
}

node::id_t graph_db::node_id_from_field(const graph_db::mapping_t &m, const std::string& str, const std::string &field) {
  std::string s(str);
  if (s[0] >= 'a' && s[0] <= 'z')
    s[0] -= 32;
  
  auto id_s = field + "_" + s;
  auto it = m.find(id_s);
  if (it == m.end()) 
    return UNKNOWN;

  return it->second;      
 }

node::id_t graph_db::node_id_from_db(const std::string& node_label, const std::string& column, const std::string &field) {
  // field -> key
  uint64_t kval = 0;
  if (is_quoted_string(field) || !is_uint64(field)) {
    auto dc = get_code(field.substr(1, field.length()-2));
    if (dc == UNKNOWN_CODE)
      return UNKNOWN;
    kval = dc;
  }
  else 
    try {
      kval = std::stoll(field);
    }
    catch (std::exception& exc) {
      spdlog::info("WARNING: cannot convert '{}' to uint64", field);
      return UNKNOWN;
    }
  
  if (has_index(node_label, column)) {
    // spdlog::info("index lookup: {}, {}", node_label, kval);
    auto idx_id = get_index(node_label, column);
    return node_id_from_index(idx_id, kval);
  }
  else {
    //scan nodes for id
    // spdlog::info("node scan: {}, {}", node_label, kval);
    auto clabel = get_code(node_label);
    auto cproperty = get_code(column);
    return get_node_id_for_property(clabel, cproperty, kval);
  }
  return UNKNOWN;
}

std::tuple<std::string, std::string> graph_db::get_rship_node(const std::string& column) {
  const auto idx = column.find_first_of("_.");
  if (std::string::npos != idx)
    return std::make_tuple(column.substr(0, idx), column.substr(idx + 1));
  else
    return std::make_tuple(column, "");
}

std::tuple<node::id_t, node::id_t> graph_db::get_connected_node_ids(const std::string& src_node, 
                                                                const std::string& src_column,
                                                                const std::string& dest_node, 
                                                                const std::string& dest_column,
                                                                aria::csv::CsvParser::iterator::reference& row,
                                                                std::optional<mapping_t>& m, std::size_t line) {
  node::id_t from_node, to_node; 
  if (m.has_value()) {   
    from_node = node_id_from_field(m.value(), src_node, row[0]);
      if (from_node == UNKNOWN) {
        spdlog::warn("mapping not found for node {} id #{}", src_node, row[0]);
        return std::make_tuple(UNKNOWN, UNKNOWN);  
      }
      to_node = node_id_from_field(m.value(), dest_node, row[1]);
      if (to_node == UNKNOWN) {
        spdlog::warn("mapping not found for node {} id #{}", dest_node, row[1]);
        return std::make_tuple(UNKNOWN, UNKNOWN);  
      }             
  }
  else {
      from_node = node_id_from_db(src_node, src_column, row[0]);
      if (from_node == UNKNOWN) {
        spdlog::warn("source node {} id #{} not found", src_node, row[0]);
        return std::make_tuple(UNKNOWN, UNKNOWN);  
      }
      to_node = node_id_from_db(dest_node, dest_column, row[1]);
      if (to_node == UNKNOWN) {
        spdlog::warn("destinstion node {} id #{} not found", dest_node, row[1]);
        return std::make_tuple(UNKNOWN, UNKNOWN);  
      }
  }
  return std::make_tuple(from_node, to_node);  
}

std::size_t graph_db::import_relationships_from_csv(const std::string &label, const std::string &filename,
                                                    char delim,
                                                    std::optional<mapping_t> &m, std::mutex *mtx) {
  using namespace aria::csv;
  std::ifstream f(filename);
  if (!f.is_open())
    return 0;

  CsvParser parser = CsvParser(f).delimiter(delim);
  std::size_t num = 0;
  std::string src_node, dest_node;
  std::string src_column, dest_column;

  std::vector<std::string> columns;

  for (auto &row : parser) {
    if (num == 0) {
      // process header
      for (auto &field : row)
        columns.push_back(field);
    
      // Column 0 is source node, column 1 is destination node in the form <Label>_<property>
      std::tie(src_node, src_column) = get_rship_node(columns[0]);
      std::tie(dest_node, dest_column) = get_rship_node(columns[1]);
      if (src_column.empty() || dest_column.empty()) {
        spdlog::warn("ERROR during relationship import: invalid source/destination column");
        return 0;
      }
    } 
    else { 
      node::id_t from_node, to_node; 
      std::tie(from_node, to_node) = get_connected_node_ids(src_node, src_column, dest_node, dest_column, row, m, num);
      properties_t props;
      auto i = 0;
      for (auto &field : row) {
        if (i > 1) {
          auto &col = columns[i];
          if (!field.empty())
            props.insert({col, field});
        }
        i++;
      }
      import_relationship(from_node, to_node, label, props);
    }
    num++;
  }

  return num-1;
}

std::size_t graph_db::import_typed_relationships_from_csv(const std::string &label, const std::string &filename,
                                                    char delim,
                                                    std::optional<mapping_t> &m, typespec_t &ty, std::mutex *mtx) {
  using namespace aria::csv;

  std::ifstream f(filename);
  if (!f.is_open()) {
    spdlog::warn("cannot find relationship file '{}'", filename);
    return 0;
  }
  CsvParser parser = CsvParser(f).delimiter(delim);
  std::size_t num = 0;
  std::string src_node, dest_node;
  std::string src_column, dest_column;

  auto label_code = dict_->insert(label);
  std::vector<std::string> columns;
  std::vector<dcode_t> prop_names;
  std::vector<p_item::p_typecode> prop_types; 
  std::vector<std::string> prop_values;

  for (auto &row : parser) {
    if (num == 0) {
      // process header
      for (auto &field : row) {
        columns.push_back(field);
      }
      // Column 0 is source node, column 1 is destination node in the form <Label>_<property>
      std::tie(src_node, src_column) = get_rship_node(columns[0]);
      std::tie(dest_node, dest_column) = get_rship_node(columns[1]);
      if (src_column.empty() || dest_column.empty()) {
        spdlog::warn("ERROR during relationship import: invalid source/destination column");
        return 0;
      }

      prop_names.resize(columns.size()-2);
      prop_types.resize(columns.size()-2);
      prop_values.resize(columns.size()-2);
      for (auto j = 2u; j < columns.size(); j++) {
        prop_names[j-2] = dict_->insert(columns[j]);
      }
      num++;
    } 
    else if (num == 1) {
      node::id_t from_node, to_node;
      std::tie(from_node, to_node) = get_connected_node_ids(src_node, src_column, dest_node, dest_column, row, m, num);

      auto i = 0, j = 0;
      for (auto &field : row) {
        if (i > 1) {
          if (!field.empty()) {
        	  prop_types[j] = get_datatype(field);
        	  prop_values[j] = field;
            j++;
          }
        }
        i++;
      }
      if (mtx != nullptr) mtx->lock();
      try {
        import_typed_relationship(from_node, to_node, label_code, prop_names, 
    		    prop_types, prop_values, dict_);
        num++;
      } catch (std::exception& exc) {
        spdlog::warn("importing relationship '{}' {} -> {} failed", label, from_node, to_node);
        continue;
      }
      if (mtx != nullptr) mtx->unlock();
    } 
    else {
      node::id_t from_node, to_node;
      std::tie(from_node, to_node) = get_connected_node_ids(src_node, src_column, dest_node, dest_column, row, m, num);

      if (mtx != nullptr) mtx->lock();
      try {
        import_typed_relationship(from_node, to_node, label_code, prop_names, 
                                prop_types, prop_values, dict_);
        num++;
      } catch (std::exception& exc) {
        spdlog::warn("importing relationship '{}' {} -> {} failed", label, from_node, to_node);
        continue;
      }
      if (mtx != nullptr) mtx->unlock();
    }
  }
  return num > 0 ? num-1 : 0;
}