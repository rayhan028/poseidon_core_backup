/*
 * Copyright (C) 2019-2022 DBIS Group - TU Ilmenau, All Rights Reserved.
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

#ifndef qplanner_hpp_
#define qplanner_hpp_

#include <boost/dll/import.hpp> 

#include "ast.hpp"
#include "query_set.hpp"

class qplanner {
public:
    qplanner() = default;
    ~qplanner() = default;

    /**
     * Transforms the given AST into a query_set which can be either
     * interpreted or compiled to LLVM code.
     */
    query_set transform(graph_db_ptr& gdb, ast_op_ptr op_tree);

    void add_udf_library(std::shared_ptr<boost::dll::shared_library> udf_lib);

private:
    /**
     * Transform the given ast recursively into a tree of qop_ptr. The source operators are 
     * collected in the sources vector. Note, that this requires a conversion from to 
     * top-to-bottom AST representation into a bottom-to-top (push-based) qop representation.
     */
    qop_ptr ast_to_qplan(graph_db_ptr& gdb, ast_op_ptr ast, std::vector<qop_ptr>& sources);

    // std::pair<qop_ptr, qop_ptr> ast_to_qset(ast_op_ptr &ast, graph_db_ptr& gdb, std::vector<qop_ptr>& sources);
    std::shared_ptr<boost::dll::shared_library> udf_lib_;

  qop_ptr node_scan_to_qplan(ast_op_ptr ast);
  qop_ptr foreach_rship_to_qplan(ast_op_ptr ast, qop_ptr child);
  qop_ptr expand_to_qplan(ast_op_ptr ast, qop_ptr child);
  qop_ptr project_to_qplan(ast_op_ptr ast, qop_ptr child);
  qop_ptr sort_to_qplan(graph_db_ptr gdb, ast_op_ptr ast, qop_ptr child);
  qop_ptr union_to_qplan(ast_op_ptr ast, qop_ptr child1, qop_ptr child2);
  qop_ptr cross_join_to_qplan(ast_op_ptr ast, qop_ptr child1, qop_ptr child2);
  qop_ptr leftouter_join_to_qplan(ast_op_ptr ast, qop_ptr child1, qop_ptr child2);

  /**
   *
   */
  properties_t jprops_to_props(const jproperty_list& jprops);
  
  template <typename T>
  qop_ptr qop_append(qop_ptr parent, std::shared_ptr<T> qop) { 
    if (parent != nullptr)
      parent->connect(qop, std::bind(&T::process, dynamic_cast<T *>(qop.get()), ph::_1, ph::_2));
    return qop;
  }

    template <typename T>
  qop_ptr qop_append2(qop_ptr parent, std::shared_ptr<T> qop) { 
    if (parent != nullptr)
      parent->connect(qop, 
        std::bind(&T::process, dynamic_cast<T *>(qop.get()), ph::_1, ph::_2), 
        std::bind(&T::finish, dynamic_cast<T *>(qop.get()), ph::_1));
    return qop;
  }
};

#endif