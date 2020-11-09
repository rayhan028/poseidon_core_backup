#include "collector.hpp"

result_collector::result_collector(unsigned int num_threads) : num_threads_(num_threads), collected_(0) {

}

void result_collector::push_result(unsigned int thread_id, qr_list *qrl) {
    rl_[thread_id].push_back(qrl);
    ++collected_;
}