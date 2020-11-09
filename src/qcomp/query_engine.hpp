#ifndef POSEIDON_CORE_QUERY_ENGINE_HPP
#define POSEIDON_CORE_QUERY_ENGINE_HPP
#include "qoperator.hpp"
#include "collector.hpp"
#include "joiner.hpp"
#include "JitFromScratch.hpp"

class base_op;

struct qscan_task {
    using task_fct = std::function<void(graph_db_ptr, unsigned)>;
    qscan_task(unsigned tid, graph_db_ptr graph, task_fct fct);

    void operator()();

    graph_db_ptr graph_;
    task_fct task_;
    unsigned tid_;
};

struct task_scheduler {

    task_scheduler(unsigned num_threads);

    void operator()();

    std::vector<qscan_task> tasks_;
    unsigned thread_num_;
    std::atomic<bool> running_;
    std::atomic<bool> started_;
    std::mutex task_mut_;
    std::condition_variable cv_;
};

class query_engine;

struct compile_task {
    compile_task(query_engine & qeng, PContext &ctx, JitFromScratch &jit, std::shared_ptr<base_op> query);

    void operator()();

    PContext &ctx_;
    JitFromScratch &jit_;
    std::shared_ptr<base_op> query_;
    query_engine &qeng_;
};
using args_el = int*;
using query_args = args_el[64];

struct arg_builder {
    std::vector<uint64_t> int_args;
    std::vector<std::string> string_args;
    std::vector<properties_t> prop_args;
    std::vector<uint64_t*> args;

    arg_builder() : args(64), string_args(64), int_args(64), prop_args(64) {}
    void arg(int op_id, std::string arg) {
        string_args[op_id] = arg;
        args[op_id] = (uint64_t*)(string_args[op_id].c_str());
    }
    void arg(int op_id, uint64_t arg) {
        int_args[op_id] = arg;
        args[op_id] = (uint64_t*)&(int_args[op_id]);
    }

    void arg(int op_id, properties_t & props) {
        prop_args[op_id] = props;
        args[op_id] = (uint64_t*)&(prop_args[op_id]);
    }



};

class query_engine {
    using qrl_collector = std::shared_ptr<result_collector>;

    unsigned thread_num_;
    qrl_collector qrlc_;
    PContext &ctx_;

    task_scheduler scheduler_;
    std::thread sched_th;
    std::thread compile_th;

    void request_next();

    transaction_ptr cur_tx_;
    std::shared_ptr<base_op> cur_query_;

public:
    query_engine(PContext &ctx, unsigned thread_num, unsigned cv_range);
    ~query_engine();

    void generate(JitFromScratch &jit, std::shared_ptr<base_op> query, bool parallel = true);

    void prepare(JitFromScratch &jit, graph_db_ptr graph);

    void run(JitFromScratch &jit, graph_db_ptr graph, result_set * rs, std::vector<uint64_t*> args);

    void run_parallel(JitFromScratch &jit, graph_db_ptr graph, result_set * rs, arg_builder & args, unsigned thread_num);

    void add_joiner(unsigned thread_id);

    void cleanup();

    bool parallel_;
    static int thread_iter_range;
    std::atomic<bool> compiled_;
    std::atomic<bool> complete_;
    std::map<int, start_ty> start_;

    std::map<int, std::vector<std::string>> operator_names_;
    std::map<int, std::vector<int>> type_vec_;
    static std::map<int, std::vector<consumer_fct_type>> operator_functions_;
    static std::map<int, finish_fct_type> finish_;


    std::function<void(transaction_ptr tx, graph_db *gdb, std::size_t first, std::size_t last, graph_db::node_consumer_func consumer)> task_callee_;

    std::vector<char*> strs;
};


#endif //POSEIDON_CORE_QUERY_ENGINE_HPP
