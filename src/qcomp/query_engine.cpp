#include <llvm/Support/Error.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/InitLLVM.h>
#include <memory>
#include "query_engine.hpp"
#include "interpreter.hpp"
#include "qid_generator.hpp"
#include "codegen_inline.hpp"
#include "thread_pool.hpp"

#include "grouper.hpp"

using namespace std::placeholders;

std::unique_ptr<p_jit> query_engine::initializeJitCompiler() {
    InitLLVM LLVM();
    InitializeNativeTarget();
	InitializeNativeTargetAsmPrinter();
	InitializeNativeTargetAsmParser();
    ExitOnError exitOnError;
    return std::make_unique<p_jit>(exitOnError);
}

std::mutex tp_mut;


query_engine::query_engine(graph_db_ptr &graph, unsigned int thread_num, unsigned cv_range) 
    : thread_num_(thread_num), 
    ctx_(PContext(graph)), 
    jit_(initializeJitCompiler()), 
    arg_counter(1u),
    graph_(graph), 
    compiled_(false), 
    complete_(false) {
        
    ctx_.getModule().setDataLayout(jit_->getDataLayout());

    auto insert_nd = [&] (graph_db* gdb, int *ptr) -> std::string {
        auto n = (node*)ptr;
        return gdb->get_node_description(n->id()).to_string();
    };

    auto insert_rd = [&] (graph_db* gdb, int *ptr) -> std::string {
        auto r = (relationship*)ptr;
        return gdb->get_rship_description(r->id()).to_string();
    };

    auto insert_int = [&] (graph_db* gdb, int *ptr) -> std::string {
        return std::to_string(*ptr);
    };

    auto insert_double = [&] (graph_db* gdb, int *ptr) -> std::string {
        return std::to_string(*reinterpret_cast<double*>(ptr));
    };

    auto insert_str = [&] (graph_db* gdb, int *ptr) -> std::string {
        return str_result[*ptr];
    };

    auto insert_uint = [&] (graph_db* gdb, int *ptr) -> std::string {
        return std::to_string(uint_result[*ptr]);
    }; 

    con_map[0] = insert_nd;
    con_map[1] = insert_rd;
    con_map[2] = insert_int;
    con_map[3] = insert_double;
    con_map[4] = insert_str;
    con_map[8] = insert_uint;
}

query_engine::~query_engine() {
    if(compile_th.joinable())
        compile_th.join();
}

void query_engine::generate(std::shared_ptr<base_op> query, bool parallel) {
    parallel_ = parallel;
    cur_query_ = query;
    compile_th = std::thread(compile_task(*this, ctx_, *jit_.get(), query));
    if(!parallel) {
        compile_th.join();
    }
    /*codegen_visitor cv(ctx_);
    query->codegen(cv);
    jit.addModule(ctx_.moveModule());*/

}

using call_map = std::array<int*, 32>;

void query_engine::cleanup() {
    start_.clear();
    operator_names_.clear();
    //operator_functions_.clear();
    finish_.clear();
    type_vec_.clear();
    complete_.store(false);
    compiled_.store(false);
    type_vec_.clear();
    qpipelines_.clear();
}

void query_engine::prepare() {
    for(auto i = 1u; i < start_.size(); i++) {
        type_vec_[i].insert(type_vec_[i].end(), type_vec_[i-1].begin(), type_vec_[i-1].end());
    }
}

joiner * last_joiner;
void query_engine::extract_arg(std::shared_ptr<base_op> op) {
    switch(op->type_) {
        case qop_type::scan: {
            auto s_op = std::dynamic_pointer_cast<scan_op>(op);
            query_args.arg(arg_counter++, s_op->label_);
            if(s_op->indexed_) {
                    //TODO: 2nd index argument
            }
            break;
        }
        case qop_type::foreach_rship: {
            auto fe_op = std::dynamic_pointer_cast<foreach_rship_op>(op);
            query_args.arg(arg_counter++, fe_op->label_);
            break;
        }
        case qop_type::expand: {
            auto exp_op = std::dynamic_pointer_cast<expand_op>(op);
            query_args.arg(arg_counter++, exp_op->label_);
            break;
        }
        case qop_type::filter: {
            //TODO: all filter arguments
            break;
        }
        case qop_type::create: {
            //TODO: all create arguments
            break;
        }
        case qop_type::cross_join:
        case qop_type::hash_join:
        case qop_type::left_join:
        case qop_type::nest_loop_join: {
            last_joiner = new joiner();
            query_args.arg(arg_counter++, last_joiner);
            break;
        }
        case qop_type::end: {
            query_args.arg(arg_counter++, last_joiner);
        }
        case qop_type::group: {
            query_args.arg(arg_counter++, new grouper()); //TODO: allocation
            query_args.arg(arg_counter++, new grouper());
            break;
        }
        // inline argument, nothing to do here
        case qop_type::project:
        case qop_type::aggr:
        case qop_type::collect:
        case qop_type::limit:
        case qop_type::sort:
        case qop_type::any:
        case qop_type::none:
        default:
            return;
    }
}


void query_engine::run(result_set * rs, arg_builder & args, bool cleanup_query) {
    //prepare();

    graph_->begin_transaction();
    current_transaction_ = current_transaction();
    if(parallel_) {
        unsigned int op_start = 1;
        arg_builder args;
        interprete_visitor iv(graph_, args, rs);
        cur_query_->codegen(iv, op_start, true);
        iv.start();
        compile_th.join();
    }
    
    int i = 0;


    auto start_idx = start_.size()-1;
    auto last = graph_->get_nodes()->num_chunks();
    query_context qtx = {graph_.get(), 0, last, current_transaction_, &rs, args.args.data()};
    qtx.gdb = graph_.get();
    qtx.rs = &rs;
    *qtx.args = *args.args.data();
    qtx.tx = current_transaction_;

    for(i = start_idx; i >= 0; i--) {
        start_[i](&qtx, args.args.data(), rs);
        //finish_[i](&qtx, args.args.data(), rs);
       for(auto finish : qpipelines_[i]) {
           finish(&qtx, args.args.data(), rs);
       }
    }

    graph_->commit_transaction();

    auto bench = qtx.profiling_time;

    std::map<int, int> operator_exec_times;
    for(auto & b : bench) {
        if(operator_exec_times.find(b.first) == operator_exec_times.end())
            operator_exec_times[b.first] = b.second;    
        else
            operator_exec_times[b.first] += b.second;
    }

    for(auto & t : operator_exec_times) {
        std::cout << "Operator: " << t.first << " : " << t.second << " ns" << std::endl;
    }

    if(cleanup_query)
        cleanup();
}

void query_engine::run(result_set * rs) {
    auto curop = cur_query_;
    while(!curop->inputs_.empty()) {
        extract_arg(curop);
        curop = curop->inputs_[0];
    }
    
    run(rs, query_args);
}

//std::map<int, std::vector<consumer_fct_type>> query_engine::operator_functions_ = {};
std::map<int, finish_fct_type> query_engine::finish_ = {};

bool has_join(algebra_optr expr) {
    auto cur = expr;

    bool join_found = false;

    while(cur->inputs_.size() > 0) {
        if(cur->type_ == qop_type::cross_join ||
           cur->type_ == qop_type::left_join) {
            join_found = true;
            break;
        }
        cur = cur->inputs_[0];
    }

    return join_found;
}

void create_tasks() {

}

void consumer_dummy(node &n) {

}
result_set rs2;
void query_engine::run_parallel(result_set * rs, arg_builder & args, unsigned thread_num) {
    bool interprete_started = false;
    scan_task::callee_ = scan_task::scan;
    interprete_visitor iv(graph_, args, &rs2);
    auto start = 1u;
    cur_query_->codegen(iv, start, false);

    auto pipeline_num = 1;

    for(auto pipeline = pipeline_num - 1; pipeline >= 0; pipeline--) {
        auto ctx = current_transaction_;
        std::thread interpreter_thread([&]{
            current_transaction_ = ctx;
            iv.start();
        });
        std::thread t1([&] {
            if(compile_th.joinable())
                    compile_th.join();
           
            task_callee_ = [&](transaction_ptr tx, graph_db *gdb, std::size_t first, std::size_t last, graph_db::node_consumer_func consumer) {
                current_transaction_ = tx;
                query_context qtx = {gdb, first, last, tx, &rs, args.args.data()};
                start_[pipeline](&qtx, args.args.data(), rs);
            };

            scan_task::callee_ = task_callee_;
            compiled_.store(true);
        });

        if(t1.joinable())
            t1.join();
        if(interpreter_thread.joinable())
           interpreter_thread.join();
        if(has_join(cur_query_) && (pipeline != pipeline_num-1)) {
            graph_->parallel_nodes(consumer_dummy);
        }
    }
}

void query_engine::finish(result_set *rs, arg_builder & args) {
    for(auto & t : rs2.data) {
        rs->append(t);
    }

    query_context qtx = {graph_.get(), 0,  graph_->get_nodes()->num_chunks(), current_transaction_, &rs, args.args.data()};
    qtx.gdb = graph_.get();
    qtx.rs = &rs;
    *qtx.args = *args.args.data();
    
    for(int i = start_.size()-1; i >= 0; i--) {
        finish_[i](&qtx, args.args.data(), rs);
    }
}

compile_task::compile_task(query_engine & qeng, PContext &ctx, p_jit &jit, std::shared_ptr<base_op> query) 
    : ctx_(ctx), jit_(jit), query_(query), qeng_(qeng) {}

int query_cnt = 0;

void compile_task::operator()() {
    //0. obtain the query identifier for the LLVM module
    qid_generator qd;
    unsigned int op_start = 1;
    unsigned int cg_start = 1;
    query_->codegen(qd, op_start, true);
    
    auto internal_qid = qd.qid;//+std::to_string(query_cnt);
    query_cnt++;

    //1. generate LLVM IR code & compile into machine code
    bool inlined = true;
    codegen_inline_visitor cv(ctx_, internal_qid);
    query_->codegen(cv, cg_start, inlined);

    //2. add LLVM module with IR to jit for compilation
    ctx_.getModule().setModuleIdentifier(internal_qid);
    jit_.get_exit()(jit_.addModule(ctx_.moveModule()));

    ctx_.createNewModule();
    ctx_.getModule().setDataLayout(jit_.getDataLayout());
    qeng_.operator_names_.clear();
    //3. extract all operator names from query and generate type vec
    //first function name == scan => start function
    auto query_id = 0;
    auto cur_op = query_;
    std::string finish_name = "default_finish_"+internal_qid;
    std::vector<int> type_vec;
    std::vector<algebra_optr> recur_list;
    qeng_.qpipelines_.clear();
    qeng_.start_.clear();
    qeng_.finish_.clear();
    
    int i = 0;
    int pipe_id = -1;
    for(auto & s : cv.pipelines) {
        if(s.find("finish_") == 0) {
            
            auto ffct = jit_.getFunctionRaw<finish_fct_type>(s);
            if(ffct) {
                qeng_.finish_[i] = *ffct;
                qeng_.qpipelines_[pipe_id].push_back(*ffct);
                i++;
            }
        } else {
            //pipe_id = std::atoi(&s.back());
            qeng_.operator_names_[0].push_back(s);
            auto start_fc = jit_.getFunctionRaw<start_ty>(s);
            if(start_fc) {
                qeng_.start_[i] = *start_fc;
                pipe_id++;
            }
        }       
    }
    
    
}
