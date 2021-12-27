/*
 * Copyright (C) 2019-2021 DBIS Group - TU Ilmenau, All Rights Reserved.
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
#ifndef mm_file_hpp_
#define mm_file_hpp_

#include <string>
#include <memory>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

class mm_file {
public:
    mm_file(const std::string& fname, std::size_t file_size);
    ~mm_file() = default;

    void close();
    void flush() { region_->flush(); }

    void *base_address() { return region_->get_address(); }
    std::size_t size() const { return region_->get_size(); }
    const std::string& file_name() const { return file_name_; }
private:
    std::string file_name_;
    std::unique_ptr<boost::interprocess::file_mapping> f_mapping_;
    std::unique_ptr<boost::interprocess::mapped_region> region_;
};

#endif