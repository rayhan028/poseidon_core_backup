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

#ifndef btree_hpp_
#define btree_hpp_

#include "defs.hpp"

#ifdef USE_PMDK

#include "pbtree.hpp"

using btree_impl = pbtree::PBPTree<uint64_t, offset_t, 50, 50>;
using btree_ptr = p_ptr<btree_impl>;

inline btree_ptr p_make_btree() { return pmem::obj::make_persistent<btree_impl>(); }

#else

#include "imbtree.hpp"

using btree_impl = imbtree::BPTree<uint64_t, offset_t, 50, 50>;
using btree_ptr = p_ptr<btree_impl>;

inline btree_ptr p_make_btree() { return std::make_shared<btree_impl>(); }

#endif

using index_id = std::size_t;

#endif