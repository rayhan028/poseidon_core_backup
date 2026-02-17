#ifndef LOADER_HPP
#define LOADER_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_set>
#include "graph_db.hpp"

// keep a local set of loaded LIDs to avoid triggering the engine's internal abort/exceptions
static std::unordered_set<uint64_t> loaded_node_lids;
static std::unordered_set<uint64_t> loaded_rship_lids;

inline std::vector<std::string> split_row(const std::string& line) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(line);
    while (std::getline(tokenStream, token, '|')) { tokens.push_back(token); }
    return tokens;
}

void load_finbench_nodes(graph_db& db, const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return;
    std::string line;
    std::getline(file, line); 
    while (std::getline(file, line)) {
        auto cols = split_row(line);
        if (cols.size() < 3) continue;
        uint64_t vt_start = std::stoull(cols[0]);
        uint64_t lid = std::stoull(cols[2]);
        db.add_node("Person", {{"name", cols[3]}}, vt_start, MAX_TIME, lid);
        loaded_node_lids.insert(lid); // Track that this exists in memory
    }
}

void load_finbench_edges(graph_db& db, const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return;
    std::string line;
    std::getline(file, line);
    uint64_t rel_lid = 8000000;
    while (std::getline(file, line)) {
        auto cols = split_row(line);
        if (cols.size() < 4) continue;
        uint64_t vt_start = std::stoull(cols[0]);
        uint64_t from_lid = std::stoull(cols[2]);
        uint64_t to_lid   = std::stoull(cols[3]);

        // PRE-FILTER: Only ask the engine for IDs exist to prevent the 'unknown_id' crash
        if (loaded_node_lids.count(from_lid) && loaded_node_lids.count(to_lid)) {
            auto src = db.try_get_node_at_vt(from_lid, vt_start);
            auto dst = db.try_get_node_at_vt(to_lid, vt_start);
            if (src && dst) {
                db.add_relationship(*src, *dst, "TRANSFER", {{"amount", cols[4]}}, vt_start, MAX_TIME, rel_lid);
                loaded_rship_lids.insert(rel_lid);
                rel_lid++;
                //db.add_relationship(*src, *dst, "TRANSFER", {{"amount", cols[4]}}, vt_start, MAX_TIME, rel_lid++);
            }
        }
    }
}
#endif