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
#include "mm_file.hpp"
#include <boost/filesystem.hpp>
#include <iostream>

namespace bip = boost::interprocess;

mm_file::mm_file(const std::string& fname, std::size_t file_size) : file_name_(fname) {
    // if file doesn't exist ...
    boost::filesystem::path path_obj(fname);
    // check if path exists and is of a regular file
    if (! boost::filesystem::exists(path_obj)) {
        // std::cout << "create new mm_file: " << fname << std::endl;
        std::filebuf fbuf;
        fbuf.open(fname, std::ios_base::in | std::ios_base::out
                            | std::ios_base::trunc | std::ios_base::binary);
        // set the size
        fbuf.pubseekoff(file_size-1, std::ios_base::beg);
        fbuf.sputc(0);
    }
    // else { std::cout << "open existing mm_file: " << fname << std::endl; }
    f_mapping_ = std::make_unique<bip::file_mapping>(fname.c_str(), bip::read_write);
    // map the whole file with read-write permissions in this process
    region_ = std::make_unique<bip::mapped_region>(*f_mapping_, bip::read_write); 
}

void mm_file::close() {
    flush();
}