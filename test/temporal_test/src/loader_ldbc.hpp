#ifndef LOADER_LDBC_HPP
#define LOADER_LDBC_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include "graph_db.hpp"

static std::unordered_map<std::string, std::unordered_map<uint64_t, uint32_t>> label_id_maps;
static uint32_t next_internal_id = 1;

inline std::vector<std::string> ldbc_split_row(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(line);
    while (std::getline(tokenStream, token, '|')) { tokens.push_back(token); }
    return tokens;
}

void load_ldbc_node(graph_db& db, const std::string& path, const std::string& label) {
    std::ifstream file(path);
    if (!file.is_open()) return;
    std::string line; std::getline(file, line); 
    int vt_s_idx = 8, vt_e_idx = 9;
    if (label == "Comment")      { vt_s_idx = 6; vt_e_idx = 7; }
    else if (label == "Post")     { vt_s_idx = 8; vt_e_idx = 9; }
    else if (label == "Forum")    { vt_s_idx = 3; vt_e_idx = 4; }
    else if (label == "Organisation" || label == "Place") { vt_s_idx = 4; vt_e_idx = 5; }
    else if (label == "Tag" || label == "TagClass")       { vt_s_idx = 3; vt_e_idx = 4; }

    int count = 0;
    auto& map = label_id_maps[label]; 
    db.begin_transaction(); 

    while (std::getline(file, line)) {
        auto cols = ldbc_split_row(line);
        if (cols.size() <= (size_t)vt_e_idx) continue;
        try {
            uint64_t ldbc_id = std::stoull(cols[0]);
            uint64_t vt_s = std::stoull(cols[vt_s_idx]);
            uint64_t vt_e = std::stoull(cols[vt_e_idx]);

            uint32_t internal_id;
            auto it = map.find(ldbc_id);
            bool is_new = (it == map.end());
            internal_id = is_new ? next_internal_id++ : it->second;       

            db.add_node(label, {}, vt_s, vt_e, internal_id);
            
            if (is_new) map[ldbc_id] = internal_id;

            if (++count % 50000 == 0) {
                db.commit_transaction();
                db.begin_transaction();
            }
        } catch (...) { continue; }
    }
    db.commit_transaction(); 
}

void load_ldbc_edge(graph_db& db, const std::string& path, const std::string& label, 
                    const std::string& src_label, const std::string& dst_label) {
    std::ifstream file(path);
    if (!file.is_open()) return;
    std::string line; std::getline(file, line); 
    
    int vt_idx = (label == "KNOWS" || label == "LIKES" || label == "WORK_AT" || label == "STUDY_AT") ? 3 : 2;
    static uint64_t rel_counter = 60000000;
    int count = 0;

    db.begin_transaction(); 
    
    while (std::getline(file, line)) {
        auto cols = ldbc_split_row(line);
        if (cols.size() <= (size_t)vt_idx) continue;
        try {
            uint64_t s_ldbc = std::stoull(cols[0]);
            uint64_t d_ldbc = std::stoull(cols[1]);

            if (label_id_maps[src_label].count(s_ldbc) == 0 || 
                label_id_maps[dst_label].count(d_ldbc) == 0) continue;

            uint32_t src_int = label_id_maps[src_label][s_ldbc];
            uint32_t dst_int = label_id_maps[dst_label][d_ldbc];
            uint64_t vt = std::stoull(cols[vt_idx]);

            auto src_opt = db.try_get_node_at_vt(src_int, vt);
            auto dst_opt = db.try_get_node_at_vt(dst_int, vt);
            if (src_opt && dst_opt) {
                db.add_relationship(*src_opt, *dst_opt, label, {}, vt, 253402300799000ULL, rel_counter++);
            }

            if (++count % 50000 == 0) {
                db.commit_transaction();
                db.begin_transaction();
            }
        } catch (...) { continue; }
    }
    db.commit_transaction(); 
}
#endif