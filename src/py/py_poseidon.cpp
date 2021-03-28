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

#include "py_poseidon.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>
#include "defs.hpp"
#include <iostream>

PYBIND11_MODULE(poseidon, m) {
    m.doc() = "poseidon graph database"; // optional module docstring

    m.def("open_pool", &graph_pool::open, py::arg("path"), py::arg("init") = false, "A function which opens the given graph pool");
    m.def("create_pool", &graph_pool::create, py::arg("path"), py::arg("size") = 1024*1024*40000ull, 
      "A function which creates a new graph pool");

    py::class_<graph_pool>(m, "GraphPool")
      //.def(py::init_alias<>())
      .def("open_graph", &graph_pool::open_graph)
      .def("create_graph", &graph_pool::create_graph);

    py::class_<graph_db, std::shared_ptr<graph_db> >(m, "Graph")
      .def("begin", &graph_db::begin_transaction)
      .def("commit", &graph_db::commit_transaction)
      .def("abort", &graph_db::abort_transaction)
      .def("create_node", [](graph_db_ptr gdb, const std::string &label, const py::dict &props) {
          properties_t node_props;
          for (auto item : props) {
            if (py::isinstance<py::str>(item.second)) {
              node_props[std::string(py::str(item.first))] = std::string(py::str(item.second)); 
            }
            else if (py::isinstance<py::int_>(item.second)) {
              node_props[std::string(py::str(item.first))] = boost::any(item.second.cast<int>());
            }
            else if (py::isinstance<py::float_>(item.second)) {
              node_props[std::string(py::str(item.first))] = boost::any(item.second.cast<double>());
            }
          }
          return gdb->add_node(label, node_props);
        })
      .def("node_by_id", &graph_db::node_by_id);
}