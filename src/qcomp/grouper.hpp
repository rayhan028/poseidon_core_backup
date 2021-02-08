#ifndef POSEIDON_CORE_GROUPER_HPP
#define POSEIDON_CORE_GROUPER_HPP
#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include "qop.hpp"

class grouper {
    static unsigned grp_cnt_;
    static int aggr_grp_cnt_;
    static std::vector<result_set> grps_;
    static std::vector<std::string> grpkey_set_;
    static std::unordered_map<std::string, unsigned> grpkey_map_;
    static std::set<unsigned> pos_set_;
    static result_set intermediate_rs_;
    static qr_tuple current_tp_;

    static bool total_grp_cnt_calc;
    static unsigned total_grp_cnt;

    static bool grp_cnt_int;
    static unsigned tota_grp_cnt_int;
public:
    grouper() {

    }
    static void add_to_group(std::string key, qr_tuple qr, std::set<unsigned> pos);
    static void finish(result_set* rs);
    static qr_tuple* demat_tuple(int index);
    static unsigned get_rs_count();

    static void init_grp_aggr();
    static unsigned get_group_cnt();
    static unsigned get_total_group_cnt();
    static unsigned get_group_sum_int(int pos);
    static double get_group_sum_double(int pos);
    static uint64_t get_group_sum_uint(int pos);


};

#endif
