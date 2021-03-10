#include "grouper.hpp"

unsigned grouper::grp_cnt_ = 0;
int grouper::aggr_grp_cnt_ = -1;
std::vector<result_set> grouper::grps_ = {};
std::vector<std::string> grouper::grpkey_set_ = {};
std::unordered_map<std::string, unsigned> grouper::grpkey_map_ = {};
std::set<unsigned> grouper::pos_set_ = {};
result_set grouper::intermediate_rs_;
qr_tuple grouper::current_tp_;

bool grouper::total_grp_cnt_calc = false;
unsigned grouper::total_grp_cnt = 0;

bool grouper::grp_cnt_int = false;
unsigned grouper::tota_grp_cnt_int = 0;

bool clear_ir = false;

void grouper::clear() {
    grp_cnt_ = 0;
    aggr_grp_cnt_ = -1;
    //grps_.clear();
    //grpkey_set_.clear();
    //grpkey_map_.clear();
    pos_set_.clear();
    //intermediate_rs_.data.clear();
    total_grp_cnt_calc = false;
    grp_cnt_int = false;
    current_tp_.clear();
}
std::mutex group_mtx; 
void grouper::add_to_group(std::string key, qr_tuple qr, std::set<unsigned> pos_set) {
    std::lock_guard<std::mutex> lck(group_mtx);
    if(clear_ir) {
        clear();
        clear_ir = false;
    }
    pos_set_ = pos_set;

    const auto itr = grpkey_map_.find(key);
    if(itr != grpkey_map_.end()) {
        grps_[itr->second].append(qr);
    } else {
        grpkey_map_.emplace(key, grp_cnt_);
        grpkey_set_.push_back(key);
        grps_.emplace_back();
        grps_[grp_cnt_++].append(qr);
    }
}

void grouper::finish(result_set* rs) {
    std::lock_guard<std::mutex> lck(group_mtx);
    rs->data.clear();
    for(auto &grp : grpkey_set_) {
        qr_tuple res;
        auto gpos = grpkey_map_[grp];
        auto tpl = grps_[gpos].data.front();
        for(auto pos : pos_set_) {
            res.push_back(tpl[pos]);
        }

        intermediate_rs_.append(res);
    }
}

qr_tuple* grouper::demat_tuple(int index) {
    std::lock_guard<std::mutex> lck(group_mtx);
    current_tp_ = intermediate_rs_.data.front();
    intermediate_rs_.data.pop_front();
    if(intermediate_rs_.data.empty()) {
        //clear();
        clear_ir = true;
        //aggr_grp_cnt_ = -1;
    }
    return &current_tp_;
}

unsigned grouper::get_rs_count() {
    return intermediate_rs_.data.size();
}
bool init = false;
void grouper::init_grp_aggr() {
    aggr_grp_cnt_++;
}

unsigned grouper::get_group_cnt() {
    auto &grp_data = grps_[aggr_grp_cnt_].data;
    auto ccnt = grp_data.size();
    grps_[aggr_grp_cnt_].data.clear();
    return ccnt;
}

unsigned grouper::get_total_group_cnt() {
    if(!total_grp_cnt_calc) {
        for(auto & g : grps_) {
            total_grp_cnt += g.data.size();
        }
        total_grp_cnt_calc = true;
        return total_grp_cnt;
    } else {
        return total_grp_cnt;
    }
}

unsigned grouper::get_group_sum_int(int pos) {
    if(!grp_cnt_int) {
        auto &grp_data = grps_[aggr_grp_cnt_].data;
        int i = 0;
        for(auto &tpl : grp_data) {
            tota_grp_cnt_int += boost::get<int>(tpl[pos]);
        }
        return tota_grp_cnt_int;
    } else {
        return tota_grp_cnt_int;
    }
}

double grouper::get_group_sum_double(int pos) {
    auto &grp_data = grps_[aggr_grp_cnt_].data;
    double gsum = 0;
    for(auto &tpl : grp_data) {
        gsum += boost::get<double>(tpl[pos]);
    }
    return gsum;
}

uint64_t grouper::get_group_sum_uint(int pos) {
    auto &grp_data = grps_[aggr_grp_cnt_].data;
    uint64_t gsum = 0;
    for(auto &tpl : grp_data) {
        gsum += boost::get<uint64_t>(tpl[pos]);
    }
    return gsum;
}