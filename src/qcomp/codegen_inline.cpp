#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include "codegen_inline.hpp"
#include "filter_expression_inline.hpp"


/*
* Function initialisation at first access path
*/
void codegen_inline_visitor::init_function(BasicBlock *bb) {

    // create function entry block
    ctx.getBuilder().SetInsertPoint(bb);

    // allocate result counter and initialize with 0
    res_counter = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    ctx.getBuilder().CreateStore(ctx.LLVM_ZERO, res_counter);
    resAlloc = ctx.getBuilder().CreateAlloca(ctx.res_arr_type);

    // forward parameters
    qr_size = ConstantInt::get(ctx.int64Ty, 1);
    sizes.push_back(qr_size);

    // rhs loop counter, used in while_loop
    // intialize with UNKNOWN ID

    rhs_alloca = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    ctx.getBuilder().CreateStore(ctx.UNKNOWN_ID, rhs_alloca);
    strAlloc = ctx.getBuilder().CreateAlloca(ctx.int8PtrTy);

    // allocate space for lhs, used in while_loop
    lhs_alloca = ctx.getBuilder().CreateAlloca(ctx.int64Ty);

    // allocate space for pitem iteration
    cur_item_arr = ctx.getBuilder().CreateAlloca(ctx.pSetRawArrTy);
    cur_item = ctx.getBuilder().CreateAlloca(ctx.pitemTy);
    cur_pset = ctx.getBuilder().CreateAlloca(ctx.propertySetPtrTy);
    pitem = ctx.getBuilder().CreateAlloca(ctx.pitemTy);
    plist_id = ctx.getBuilder().CreateAlloca(ctx.int64Ty);

    new_res_arr_type = ArrayType::get(ctx.int64PtrTy, new_type_vec.size() + 1);
    newResAlloc = ctx.getBuilder().CreateAlloca(new_res_arr_type);

    // allocate space for each projection variable
    for(auto & i : pv)
        i = ctx.getBuilder().CreateAlloca(ctx.int64Ty);

    // create vector of projection keys
    project_keys.resize(project_string.size());
    int i = 0;
    for(auto & s : project_string) {
        auto prj_alloc = ctx.getBuilder().CreateAlloca(ctx.int8PtrTy);
        GlobalVariable *label = ctx.getBuilder().CreateGlobalString(StringRef(s), "pkey_"+s);
        auto str_ptr = ctx.getBuilder().CreateBitCast(label, ctx.int8PtrTy);
        ctx.getBuilder().CreateStore(str_ptr, prj_alloc);
        project_keys[i] = ctx.getBuilder().CreateLoad(prj_alloc);
        i++;
    }

    // allocate space for iteration variables
    max_idx = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    max_cnt = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    ctx.getBuilder().CreateStore(ctx.MAX_PITEM_CNT, max_cnt);
    //cur_idx = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    loop_cnt = ctx.getBuilder().CreateAlloca(ctx.int64Ty);

    cross_join_idx.push_back(ctx.getBuilder().CreateAlloca(ctx.int64Ty));
    //cross_join_rship = ctx.getBuilder().CreateAlloca(ctx.rshipPtrTy);
    dangling = ctx.getBuilder().CreateAlloca(ctx.int64Ty);    
}

void codegen_inline_visitor::init_finish(BasicBlock *bb) {
    // create function entry block
    ctx.getBuilder().SetInsertPoint(bb);
}


AllocaInst *codegen_inline_visitor::insert_alloca(Type *ty) {
    //get and save current basic block
    auto cur_bb = ctx.getBuilder().GetInsertBlock();

    // move to init basic block
    ctx.getBuilder().SetInsertPoint(inits);

    // check if block has terminator
    auto term = inits->getTerminator();
    if(term) {
        // unlink temrinator from basic block
        term->removeFromParent();
    } 

    // insert new alloca
    auto alloc = ctx.getBuilder().CreateAlloca(ty);

    // insert old terminator again
    if(term) {
        ctx.getBuilder().Insert(term);
    }

    // set insert point to saved basic block
    ctx.getBuilder().SetInsertPoint(cur_bb);

    return alloc;
}
int qcnt = 0;

/*
* Generates code for a scan operator.
*/
void codegen_inline_visitor::visit(std::shared_ptr<scan_op> op) {
    
    // clear register mapping of previous access path
    reg_query_results.clear();
    // process the appropriate query length
    query_length(op);

    // create unique function name for the JIT instance
    int cnt = 0;

    op->name_ = qid_+std::to_string(qcnt++);
    std::string finish_name = "finish_"+op->name_;
    //}

    query_id_str = op->name_;
    main_function = Function::Create(ctx.startFctTy, Function::ExternalLinkage, op->name_, ctx.getModule());
    main_finish   = Function::Create(ctx.finishFctTy, Function::ExternalLinkage, finish_name, ctx.getModule());

    pipelines.push_back(std::string(main_function->getName()));
    pipelines.push_back(std::string(main_finish->getName()));

    cur_pipeline = main_function;
    // obtain all relevant function callees for the scan operator
    //FunctionCallee dict_lookup_label = ctx.extern_func("dict_lookup_label");
    auto get_valid_node = ctx.extern_func("get_valid_node");
    auto gdb_get_nodes = ctx.extern_func("gdb_get_nodes");
    auto gdb_get_node_from_it = ctx.extern_func("get_node_from_it");
    auto get_begin = ctx.extern_func("get_vec_begin");
    auto get_next = ctx.extern_func("get_vec_next");
    auto is_end = ctx.extern_func("vec_end_reached");
    auto fadd_now = ctx.extern_func("get_now");
    auto fadd_time_diff = ctx.extern_func("add_time_diff");

    /*
    * When a previous access path already exists, push the new entry block as the first block into the
    * function block list and link it as the finish block
    */
    BasicBlock *bb;
    BasicBlock *next_op;

    inits = BasicBlock::Create(ctx.getModule().getContext(), "inits_entry", main_function);
    bb = BasicBlock::Create(ctx.getModule().getContext(), "entry", main_function);

    entry = bb;

    // initialize all relevant functions
    init_function(inits);

    scan_nodes_end = BasicBlock::Create(ctx.getModule().getContext(), "scan_nodes_end", main_function);
    BasicBlock *consumeBB = BasicBlock::Create(ctx.getModule().getContext(), "consume_node", main_function);

    df_finish_bb = BasicBlock::Create(ctx.getContext(), "finish_entry", main_finish);

    // obtain the query context & function arguments

    query_context = main_function->args().begin();
    queryArgs = main_function->args().begin() + 1;
    rs = main_function->args().begin() + 2;

    BasicBlock *curBB;

    //if(access)
    //else 
    extract_query_context(query_context);
    ctx.getBuilder().CreateBr(bb);

    ctx.getBuilder().SetInsertPoint(bb);
    
    
    // register for the current node
    Value *node;

    Value *t_start;
    Value *t_end;

    // scan all
    if(!op->indexed_) {
        // SCAN NODES
        // 1 get nodes vector from graph db
        auto nodes_vec = ctx.getBuilder().CreateCall(gdb_get_nodes, {gdb});

        // 3 obtain iterator from nodes vector
        auto node_begin_it = ctx.getBuilder().CreateCall(get_begin, {nodes_vec, first, last});

        // 2 lookup single label in dict
        if(op->labels_.empty()) {
            auto label_code = ctx.extract_arg_label(op->op_id_, gdb, queryArgs);

            // 4 iterate through node list
            curBB = ctx.while_loop(main_function, get_begin, get_next, is_end, node_begin_it, nodes_vec,
                                            scan_nodes_end, [&](BasicBlock *curBB, BasicBlock *epilog) {
                        if(profiling)
                            t_start = ctx.getBuilder().CreateCall(fadd_now, {});
                        // 5 obtain node from iterator
                        auto node_raw = ctx.getBuilder().CreateCall(gdb_get_node_from_it, {node_begin_it});

                        // 6 obtain valid node
                        node = ctx.getBuilder().CreateCall(get_valid_node, {gdb, node_raw, tx_ptr});

                        // 7 compare labels
                        auto cond = ctx.node_cmp_label(node, label_code);

                        // 8 if equal -> consume
                        ctx.getBuilder().CreateCondBr(cond, consumeBB, epilog);
                    }, consumeBB);
        } else {
            FunctionCallee dict_lookup_label = ctx.extern_func("dict_lookup_label");
            auto opid = ConstantInt::get(ctx.int64Ty, op->op_id_);
            
            std::vector<Value*> label_codes;
            std::vector<BasicBlock*> multi_label_conds;
            auto i = 0u;
            Value *id = opid;
            for(auto & label : op->labels_) {
                auto str = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(queryArgs, {ctx.LLVM_ZERO, id}));
                label_codes.push_back(ctx.getBuilder().
                                        CreateCall(dict_lookup_label, 
                                        {gdb, ctx.getBuilder().CreateBitCast(str, ctx.int8PtrTy)}
                                        ));
                id = ctx.getBuilder().CreateAdd(id, ctx.LLVM_ONE);
                multi_label_conds.push_back(BasicBlock::Create(ctx.getModule().getContext(), "condition_"+label, main_function));
            }

            BasicBlock *loop_epilog;
            curBB = ctx.while_loop(main_function, get_begin, get_next, is_end, node_begin_it, nodes_vec,
                                            scan_nodes_end, [&](BasicBlock *curBB, BasicBlock *epilog) {
                        loop_epilog = epilog;
                        // 5 obtain node from iterator
                        auto node_raw = ctx.getBuilder().CreateCall(gdb_get_node_from_it, {node_begin_it});

                        // 6 obtain valid node
                        node = ctx.getBuilder().CreateCall(get_valid_node, {gdb, node_raw, tx_ptr});

                        ctx.getBuilder().CreateBr(multi_label_conds.front());
                    }, consumeBB);


            i = 0;
            for(auto cond : multi_label_conds) {
                ctx.getBuilder().SetInsertPoint(cond);
                auto label_cmp = ctx.node_cmp_label(node, label_codes[i++]);
                if(i == multi_label_conds.size()) {
                    ctx.getBuilder().CreateCondBr(label_cmp, consumeBB, loop_epilog);
                } else {
                    ctx.getBuilder().CreateCondBr(label_cmp, consumeBB, multi_label_conds[i]);
                }

            }

        }


    } else { // index scan

        // for index scan call extern gdb function and branch to consumer
        auto get_index_node = ctx.extern_func("index_get_node");

        // get label and property key from parameter descriptor
        auto opid = ConstantInt::get(ctx.int64Ty, op->op_id_);
        auto prop_opid = ConstantInt::get(ctx.int64Ty, op->op_id_+1);
        auto val_opid = ConstantInt::get(ctx.int64Ty, op->op_id_+2);
        auto label = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(queryArgs, {ctx.LLVM_ZERO, opid}));
        auto prop = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(queryArgs, {ctx.LLVM_ZERO, prop_opid}));
        auto val = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(queryArgs, {ctx.LLVM_ZERO, val_opid})));

        // call extern function to obtain node
        node = ctx.getBuilder().CreateCall(get_index_node, {gdb,
                                                            ctx.getBuilder().CreateBitCast(label, ctx.int8PtrTy),
                                                            ctx.getBuilder().CreateBitCast(prop, ctx.int8PtrTy), val});

        // check if node ptr is null
        auto nullptr_cmp = ctx.getBuilder().CreateIsNotNull(node);

    	// consume or goto finish
        ctx.getBuilder().CreateCondBr(nullptr_cmp, consumeBB, scan_nodes_end);
        curBB = scan_nodes_end;
    }

    // branch to the next oeprator
    ctx.getBuilder().SetInsertPoint(consumeBB);
    {
        if(profiling) {
            t_end = ctx.getBuilder().CreateCall(fadd_now, {});
            ctx.getBuilder().CreateCall(fadd_time_diff, {query_context, ConstantInt::get(ctx.int64Ty, op->op_id_), t_start, t_end});
        }
        // add current node to register
        reg_query_results.push_back({node, 0});
        main_return = curBB;
        // next operator fills this basic block and adds terminator
    }

    // branch to the finish function
    ctx.getBuilder().SetInsertPoint(scan_nodes_end);
    {   
        global_end = scan_nodes_end;
        //ctx.getBuilder().CreateCall(ctx.finishFctTy, finish, {rs});
        ctx.getBuilder().CreateRetVoid();
        //ctx.getBuilder().CreateBr(df_finish_bb);
    }

    // set appropriate backflow
    prev_bb = consumeBB;
    ctx.gen_funcs[op->name_] = main_function;
}

/**
 * Generates code for the foreach relationship operator.
 */
void codegen_inline_visitor::visit(std::shared_ptr<foreach_rship_op> op) {
    op->name_ = "";
    
    // obtain the relationship field ids from the direction
    int node_rship_idx = 2;
    int next_rship_idx = 4;
    switch (op->dir_) {
        case RSHIP_DIR::FROM:
            node_rship_idx = 2;
            next_rship_idx = 4;
            break;
        case RSHIP_DIR::TO:
            node_rship_idx = 3;
            next_rship_idx = 5;
    }

    // obtain the FunctionCallees for the operator processing
    auto rship_by_id = ctx.extern_func("rship_by_id");
    //FunctionCallee gdb_get_rships = ctx.extern_func("gdb_get_rships");
    //FunctionCallee ohop_count = ctx.extern_func("count_potential_o_hop");
    //FunctionCallee get_rship_queue = ctx.extern_func("retrieve_fev_queue");
    //FunctionCallee insert_to_queue = ctx.extern_func("insert_fev_rship");

    auto retrieve_rships = ctx.extern_func("foreach_from_variable_rship");
    auto get_next_rship = ctx.extern_func("get_next_rship_fev");
    auto fev_list_end = ctx.extern_func("fev_list_end");
    auto fadd_now = ctx.extern_func("get_now");
    auto fadd_time_diff = ctx.extern_func("add_time_diff");

    // create the basic blocks for the operator processing
    BasicBlock *fe_entry = BasicBlock::Create(ctx.getContext(), "fe_entry", cur_pipeline);
    BasicBlock *foreach_rship_end = BasicBlock::Create(ctx.getModule().getContext(), "foreach_rel_end", cur_pipeline);
    BasicBlock *nextBB = BasicBlock::Create(ctx.getModule().getContext(), "next_rship", cur_pipeline);
    BasicBlock *consumeBB = BasicBlock::Create(ctx.getModule().getContext(), "consume_rship", cur_pipeline);

    // basic blocks for the fev operator
    BasicBlock *loop_head = BasicBlock::Create(ctx.getContext(), "fev_loop_head", cur_pipeline);
    BasicBlock *loop_body = BasicBlock::Create(ctx.getContext(), "fev_loop_body", cur_pipeline);
    //BasicBlock *loop_next = BasicBlock::Create(ctx.getContext(), "fev_loop_next", main_function);

    // link the entry point with the block of the previous operator
    ctx.getBuilder().SetInsertPoint(prev_bb);
    ctx.getBuilder().CreateBr(fe_entry);
    ctx.getBuilder().SetInsertPoint(fe_entry);

    Value* t_start = nullptr;
    Value* t_end = nullptr;

    Value *node = nullptr;

    // get the previous tuple result
    if(op->node_pos_ == std::numeric_limits<int>::max())
        node = reg_query_results.back().reg_val;
    else
        node = reg_query_results.at(op->node_pos_).reg_val;

    auto UNKNOWN_REL_ID = ConstantInt::get(ctx.int64Ty,
                                           std::numeric_limits<int64_t>::max()); // TODO: UNKNOWN VALUE = std::numeric_limits<int64_t>::max()
    auto nr = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    ctx.getBuilder().CreateStore(UNKNOWN_REL_ID, nr);

    GlobalVariable *label = ctx.getBuilder().CreateGlobalString(StringRef(op->label_), "rship_label");

    auto strPtr = ctx.getBuilder().CreateBitCast(label, ctx.int8PtrTy);
    auto stra = ctx.getBuilder().CreateAlloca(ctx.int8PtrTy);
    ctx.getBuilder().CreateStore(strPtr, stra);

    //auto label_code = ConstantInt::get(ctx.int32Ty, APInt(32, ctx.get_dcode(op->label_)));

    // extract label from arguments and obtain label code
    auto qctx = cur_pipeline->args().begin();
    auto g = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(qctx, 0));
    auto args = cur_pipeline->args().begin()+1;
    auto label_code = ctx.extract_arg_label(op->op_id_, g, args);

    // extract the first relationship id from the node
    auto node_rship_id = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(node, node_rship_idx));
    auto lhs = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    ctx.getBuilder().CreateStore(node_rship_id, lhs);
    
    // register value for the current relationship
    Value *rship;

    // generate code depending on variable traversion or single
    if(op->is_variable()) {
        // for variable traversion
        // transform min and max hop into constants
        auto min_hop = ConstantInt::get(ctx.int64Ty, op->hops_.first);
        auto max_hop = ConstantInt::get(ctx.int64Ty, op->hops_.second);

        // retrieve all relationships and store it into a intermediate vector (externally)
        ctx.getBuilder().CreateCall(retrieve_rships, {gdb, label_code, node, min_hop, max_hop});        

        // simple loop in order to traverse the previous filled vector
        ctx.getBuilder().CreateBr(loop_head);
        // the loop header
        ctx.getBuilder().SetInsertPoint(loop_head);
        // check if the end of the vector is reached (externally) and branch to body or end
        auto is_end = ctx.getBuilder().CreateCall(fev_list_end, {});
        ctx.getBuilder().CreateCondBr(is_end, foreach_rship_end, loop_body);

        // in the body, simply obtain the relationship (externally) and move the address to a register value
        ctx.getBuilder().SetInsertPoint(loop_body);
        rship = ctx.getBuilder().CreateCall(get_next_rship, {});

        // branch to the next operator
        ctx.getBuilder().CreateBr(consumeBB);

    } else {
        // iterate through the relationship list of the node
        auto loop_body = ctx.while_loop_condition(cur_pipeline, lhs, nr, PContext::WHILE_COND::LT, foreach_rship_end,
                                                [&](BasicBlock *, BasicBlock *) {
                                                    if(profiling)
                                                        t_start = ctx.getBuilder().CreateCall(fadd_now, {});

                                                    // extract relationship id from alloca
                                                    auto lhsi = ctx.getBuilder().CreateLoad(lhs);

                                                    // obtain the relationship through an extern function call
                                                    rship = ctx.getBuilder().CreateCall(rship_by_id, {g, lhsi});

                                                    // check for the given label
                                                    auto consume_cond = ctx.rship_cmp_label(rship, label_code);

                                                    // branch if relationship label is equal to the given label
                                                    ctx.getBuilder().CreateCondBr(consume_cond, consumeBB, nextBB);
                                                });

        // obtain the next relationship 
        ctx.getBuilder().SetInsertPoint(nextBB);
        {
            auto next_rship = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(rship, next_rship_idx));
            ctx.getBuilder().CreateStore(next_rship, lhs);
            ctx.getBuilder().CreateBr(loop_body);
        }
    }

    // consume the given relationship
    ctx.getBuilder().SetInsertPoint(consumeBB);
    {
        if(profiling) {
            t_end = ctx.getBuilder().CreateCall(fadd_now, {});
            ctx.getBuilder().CreateCall(fadd_time_diff, {query_context, ConstantInt::get(ctx.int64Ty, op->op_id_), t_start, t_end});
        }

        reg_query_results.push_back({rship, 1});
        prev_bb = consumeBB;
    }

    // backflow after complete iteration to the previous operator
    ctx.getBuilder().SetInsertPoint(foreach_rship_end);
    {
        // return to main loop of scan
        ctx.getBuilder().CreateBr(main_return);
    }

    if(op->is_variable()) {
        main_return = loop_head;
    } else {
        // set backflow to the iteration loop
        main_return = nextBB;
    }

}

/**
 * Generates code for a projection operator
 */ 
void codegen_inline_visitor::visit(std::shared_ptr<project> op) {
    op->name_ = "";

    // obtain FunctionCallee for apply_pexpr
    //auto apply_pexpr = ctx.extern_func("apply_pexpr");
    auto apply_pexpr_node = ctx.extern_func("apply_pexpr_node");
    auto apply_pexpr_rship = ctx.extern_func("apply_pexpr_rship");
    auto node_has_property = ctx.extern_func("node_has_property");
    auto rship_has_property = ctx.extern_func("rship_has_property");
    auto apply_has_property = ctx.extern_func("apply_has_property");
    auto fadd_now = ctx.extern_func("get_now");
    auto fadd_time_diff = ctx.extern_func("add_time_diff");

    // create entry block and link with the previous operator
    BasicBlock *entry = BasicBlock::Create(ctx.getContext(), "project_entry", main_function);
    ctx.getBuilder().SetInsertPoint(prev_bb);
    ctx.getBuilder().CreateBr(entry);
    ctx.getBuilder().SetInsertPoint(entry);
    std::map<std::size_t, Value*> vmap;

    Value* t_start = nullptr;
    Value* t_end = nullptr;

    auto i = 1;

    if(profiling)
        t_start = ctx.getBuilder().CreateCall(fadd_now, {});
        
    // add new projection variables to registers
    std::vector<QR_VALUE> nqrv;
    for(auto & pe : op->prexpr_) {
        auto r = reg_query_results[pe.id];
        if(pe.prt == pr_expr::PROJECTION_TYPE::PROPERTY_PR) {
            if(pe.type == FTYPE::BOOLEAN) {
                nqrv.push_back(reg_query_results[pe.id]);
            } else {
                
                // get register value of tuple for projection
                auto qrp = ctx.getBuilder().CreateBitCast(r.reg_val, ctx.int64PtrTy);

                // get the appropriate type of the tuple
                auto type = ConstantInt::get(ctx.int64Ty, APInt(64, static_cast<int>(pe.type)));

                // apply the projection in an external function
                if(r.type == 0) {
                    ctx.getBuilder().CreateCall(apply_pexpr_node, {gdb, project_keys[i-1], type, qrp, pv[i]});
                } else {
                    ctx.getBuilder().CreateCall(apply_pexpr_rship, {gdb, project_keys[i-1], type, qrp, pv[i]});
                }

                // add new register value to global list
                nqrv.push_back({pv[i], static_cast<int>(pe.type)+2});

            }
        } else if(pe.prt == pr_expr::PROJECTION_TYPE::FORWARD_PR) {
            nqrv.push_back({r.reg_val, r.type});
            i++;
            continue;
        } else if(pe.prt == pr_expr::PROJECTION_TYPE::FUNCTIONAL_VAL) {
            pe.type = FTYPE::INT;
            auto fc_raw = ConstantInt::get(ctx.int64Ty, (int64_t)pe.int_node_func);
            auto fc_ptr = ctx.getBuilder().CreateIntToPtr(fc_raw, ctx.int64PtrTy);
            auto fc_ty = FunctionType::get(ctx.int64Ty, {ctx.nodePtrTy}, false);
            auto fc = ctx.getBuilder().CreateBitCast(fc_ptr, fc_ty->getPointerTo());

            auto i_pr = ctx.getBuilder().CreateCall(fc_ty, fc, {r.reg_val});
            ctx.getBuilder().CreateStore(i_pr, pv[i]);
            nqrv.push_back({pv[i], static_cast<int>(FTYPE::INT)+2});
        } else if(pe.prt == pr_expr::PROJECTION_TYPE::CONDITIONAL_VAL) {
            std::vector<Value*> prop_exists;
            std::vector<Value*> then_else;

            auto r  = reg_query_results[pe.id];

            for(auto & prop_str : pe.has_properties) {
                auto str_ptr = ctx.getBuilder().CreateGlobalStringPtr(prop_str);

                if(r.type == 0) {
                    prop_exists.emplace_back(
                        ctx.getBuilder().CreateCall(node_has_property, {gdb, r.reg_val, str_ptr}));
                } else {
                    prop_exists.emplace_back(
                        ctx.getBuilder().CreateCall(rship_has_property, {gdb, r.reg_val, str_ptr}));
                }
            }


            std::vector<Value*> add_sum;
            add_sum.push_back(ctx.LLVM_ZERO);
            for(auto p : prop_exists) {
                auto lhs = add_sum.back();
                add_sum.push_back(ctx.getBuilder().CreateAdd(lhs,p));
            }
            
            auto str_ptr_st = ctx.getBuilder().CreateGlobalStringPtr(pe.then_else.first);
            auto str_ptr_nd = ctx.getBuilder().CreateGlobalStringPtr(pe.then_else.second);
            
            ctx.getBuilder().CreateCall(apply_has_property, {add_sum.back(), str_ptr_st, str_ptr_nd, pv[i]});
            nqrv.push_back({pv[i], 4});
        }
        i++;
    }

    if(profiling) {
        t_end = ctx.getBuilder().CreateCall(fadd_now, {});
        ctx.getBuilder().CreateCall(fadd_time_diff, {query_context, ConstantInt::get(ctx.int64Ty, op->op_id_), t_start, t_end});
    }

    // switch old register list with the new one
    reg_query_results = nqrv;

    prev_bb = entry;
}

/**
 * Generates code for the expand operator
 */ 
void codegen_inline_visitor::visit(std::shared_ptr<expand_op> op) {
    // get the id of the appropriate expand operation (to or from rship)
    unsigned exp_id;
    switch (op->exp_) {
        case EXPAND::OUT:
            exp_id = 3;
            break;
        case EXPAND::IN:
            exp_id = 2;
            break;
    }
    op->name_ = "";

    // create the basic blocks of the expand operator
    BasicBlock *entry = BasicBlock::Create(ctx.getContext(), "expand_entry", cur_pipeline);
    BasicBlock *consume = BasicBlock::Create(ctx.getContext(), "expand_consume", cur_pipeline);
    BasicBlock *null = BasicBlock::Create(ctx.getContext(), "expand_false", cur_pipeline);
    BasicBlock *check_label = BasicBlock::Create(ctx.getContext(), "check_label", cur_pipeline);

    Value *label_code = nullptr;
    Value* t_start = nullptr;
    Value* t_end = nullptr;

    // get the relevant FunctionCallee
    auto get_node_by_id = ctx.extern_func("node_by_id");
    auto fadd_now = ctx.extern_func("get_now");
    auto fadd_time_diff = ctx.extern_func("add_time_diff");

    // link with the previous operator
    ctx.getBuilder().SetInsertPoint(prev_bb);
    ctx.getBuilder().CreateBr(entry);
    ctx.getBuilder().SetInsertPoint(entry);

    if(profiling)
        t_start = ctx.getBuilder().CreateCall(fadd_now, {});

    // obtain the last query result from register -> relationship
    auto rship = reg_query_results.back().reg_val;

    // extract the node id from the relationship
    auto exp_rship = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(rship, exp_id));

    auto qctx = cur_pipeline->args().begin();
    auto g = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(qctx, 0));


    //get node by id
    auto node = ctx.getBuilder().CreateCall(get_node_by_id, {g, exp_rship});
    qr.push_back(node);

    // test if node is null
    auto null_node = Constant::getNullValue(ctx.int8PtrTy);
    auto cmp_node = ctx.getBuilder().CreateBitCast(node, ctx.int8PtrTy);
    auto cmp = ctx.getBuilder().CreateICmpEQ(null_node, cmp_node);

    // branch to different positions, when node label is given
    if(op->label_.empty())
        ctx.getBuilder().CreateCondBr(cmp, null, consume);
    else {
        //label_code = ctx.extract_arg_label(op->op_id_, gdb, queryArgs);
        auto args = cur_pipeline->args().begin()+1;
        label_code = ctx.extract_arg_label(op->op_id_, g, args);
        ctx.getBuilder().CreateCondBr(cmp, null, check_label);
    }

    // node label is given
    if(!op->label_.empty())
    {
        ctx.getBuilder().SetInsertPoint(check_label);
        {
            //auto label_code = ConstantInt::get(ctx.int32Ty, APInt(32, ctx.get_dcode(op->label_)));
            // compare labels 
            auto cond = ctx.node_cmp_label(node, label_code);

            // branch to appropriate block
            ctx.getBuilder().CreateCondBr(cond, consume, null);
        }
    } else if(!op->labels_.empty()) {        
        FunctionCallee dict_lookup_label = ctx.extern_func("dict_lookup_label");
        ctx.getBuilder().SetInsertPoint(check_label);
        auto opid = ConstantInt::get(ctx.int64Ty, op->op_id_);
            
        std::vector<Value*> label_codes;
        std::vector<BasicBlock*> multi_label_conds;
        auto i = 0u;
        Value *id = opid;
        for(auto & label : op->labels_) {
            auto str = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(queryArgs, {ctx.LLVM_ZERO, id}));
            label_codes.push_back(ctx.getBuilder().
                                     CreateCall(dict_lookup_label, 
                                        {gdb, ctx.getBuilder().CreateBitCast(str, ctx.int8PtrTy)}
                                        ));
            id = ctx.getBuilder().CreateAdd(id, ctx.LLVM_ONE);
            multi_label_conds.push_back(BasicBlock::Create(ctx.getModule().getContext(), "condition_"+label, main_function));
        }

        ctx.getBuilder().CreateBr(multi_label_conds.front());

        i = 0;
        for(auto cond : multi_label_conds) {
            ctx.getBuilder().SetInsertPoint(cond);
            auto label_cmp = ctx.node_cmp_label(node, label_codes[i++]);
            if(i == multi_label_conds.size()) {
                ctx.getBuilder().CreateCondBr(label_cmp, consume, null);
            } else {
                ctx.getBuilder().CreateCondBr(label_cmp, consume, multi_label_conds[i]);
            }

        }

    }

    // add result to register list 
    ctx.getBuilder().SetInsertPoint(consume);
    {
        if(profiling) {
            t_end = ctx.getBuilder().CreateCall(fadd_now, {});
            ctx.getBuilder().CreateCall(fadd_time_diff, {query_context, ConstantInt::get(ctx.int64Ty, op->op_id_), t_start, t_end});
        }
        reg_query_results.push_back({node, 0});

        prev_bb = consume;
        // next operator
    }

    ctx.getBuilder().SetInsertPoint(null);
    {
        ctx.getBuilder().CreateBr(main_return);
    }
}

/**
 * Generates code for the filter operator
 */ 
void codegen_inline_visitor::visit(std::shared_ptr<filter_op> op) {
    op->name_ = "";

    // create all relevant basic blocks for the operator processing
    BasicBlock *entry = BasicBlock::Create(ctx.getContext(), "filter_entry", main_function);
    BasicBlock *consume = BasicBlock::Create(ctx.getContext(), "filter_consume", main_function);
    BasicBlock *false_pred = BasicBlock::Create(ctx.getContext(), "filter_false", main_function);

    auto fadd_now = ctx.extern_func("get_now");
    auto fadd_time_diff = ctx.extern_func("add_time_diff");

    // link with the previous operator
    ctx.getBuilder().SetInsertPoint(prev_bb);
    ctx.getBuilder().CreateBr(entry);
    ctx.getBuilder().SetInsertPoint(entry);

    Value* t_start = nullptr;
    Value* t_end = nullptr;

    if(profiling)
        t_start = ctx.getBuilder().CreateCall(fadd_now, {});

    // extract the argument from the argument desc
    auto opid = ConstantInt::get(ctx.int64Ty, op->op_id_);
    auto arg = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(queryArgs, {ctx.LLVM_ZERO, opid})));
    
    // obtain the last query result
    //auto pres = reg_query_results.back().reg_val;
    
    // generate code for each filter expression
    auto vis = fep_visitor_inline(&ctx, main_function, reg_query_results, consume, 
            false_pred, cur_item_arr, cur_item, cur_pset, 
            rhs_alloca, plist_id, 
            loop_cnt, max_cnt, pitem, arg);
    op->fexpr_->accept(0, vis);


    ctx.getBuilder().SetInsertPoint(consume);
    {
        if(profiling) {
            t_end = ctx.getBuilder().CreateCall(fadd_now, {});
            ctx.getBuilder().CreateCall(fadd_time_diff, {query_context, ConstantInt::get(ctx.int64Ty, op->op_id_), t_start, t_end});
        }
        // next operator
        prev_bb = consume;
    }

    ctx.getBuilder().SetInsertPoint(false_pred);
    {
        //return to main loop
        ctx.getBuilder().CreateBr(main_return);
    }
}

/**
 * Generates code for the collection
 */ 
void codegen_inline_visitor::visit(std::shared_ptr<collect_op> op) {
    cur_size = op->op_id_;
    op->name_ = "";

    // obtain all relevant FunctionCallees
    auto mat_reg = ctx.extern_func("mat_reg_value");
    auto collect_regs = ctx.extern_func("collect_tuple");
    auto fadd_now = ctx.extern_func("get_now");
    auto fadd_time_diff = ctx.extern_func("add_time_diff");
    auto endnotify = ctx.extern_func("end_notify");

    Value *gdb_ptr;
    Value *rs_arg;
    Function *collect_fct;

    if(pipelined_finish) {
        collect_fct = main_finish;
        rs_arg = main_finish->args().begin()+2;
    } else {
        collect_fct = main_function;
        rs_arg = rs;
        gdb_ptr = gdb;
    }

    // link with previous operator
    BasicBlock *entry = BasicBlock::Create(ctx.getContext(), "collect_entry", collect_fct);
    pre_tuple_mat = BasicBlock::Create(ctx.getModule().getContext(), "pre_tuple_mat", collect_fct);
    ctx.getBuilder().SetInsertPoint(prev_bb);
    ctx.getBuilder().CreateBr(entry);
    ctx.getBuilder().SetInsertPoint(entry);

    if(pipelined_finish) {
        gdb_ptr = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(collect_fct->args().begin(), 0));
    }

    Value* t_start = nullptr;
    Value* t_end = nullptr;

    if(profiling) 
        t_start = ctx.getBuilder().CreateCall(fadd_now, {});

    auto print_tuple = ConstantInt::get(ctx.boolTy, op->print_on_collect_);

    // create a single materialization call for each register value
    // each register will be materialized into thread local storage
    for(auto & res : reg_query_results) {
        // value type 7 => boolean, already transformed into integer
        auto type_id = (res.type == 7 ? 2 : res.type);
        auto type = ConstantInt::get(ctx.int64Ty, type_id);
        auto cv_reg = ctx.getBuilder().CreateBitCast(res.reg_val, ctx.int64PtrTy);
        ctx.getBuilder().CreateCall(mat_reg, {gdb_ptr, cv_reg, type});
    }

    ctx.getBuilder().CreateBr(pre_tuple_mat);

    ctx.getBuilder().SetInsertPoint(pre_tuple_mat);
    // materialize the complete thread local storage into a global list
    ctx.getBuilder().CreateCall(collect_regs, {gdb_ptr, rs_arg, print_tuple});

    if(profiling) {
        t_end = ctx.getBuilder().CreateCall(fadd_now, {});
        ctx.getBuilder().CreateCall(fadd_time_diff, {query_context, ConstantInt::get(ctx.int64Ty, op->op_id_), t_start, t_end});
    }
    
    ctx.getBuilder().CreateBr(main_return);

    // complete the finish call
    ctx.getBuilder().SetInsertPoint(df_finish_bb);
    //ctx.getBuilder().CreateCall(endnotify, {rs});
    ctx.getBuilder().CreateRetVoid();

    // complete init->entry
    //if(!pipelined) {    
    //    ctx.getBuilder().SetInsertPoint(inits);
    //    ctx.getBuilder().CreateBr(this->entry);}
}

// process recursively the type vector of the rhs of a join
void get_rhs_type(std::shared_ptr<base_op>  &qop, std::vector<int> &typv) {
    auto op = qop;
    while(op->inputs_.size() > 0) {
        if(op->type_ == qop_type::scan)
            typv.push_back(0);
        else if(op->type_ == qop_type::foreach_rship)
            typv.push_back(1);
        else if(op->type_ == qop_type::expand)
            typv.push_back(0);
        else if(op->type_ == qop_type::project) {
            auto prj = std::dynamic_pointer_cast<project>(op);
            std::vector<int> new_types;
            for(auto & pe : prj->prexpr_) {
                new_types.push_back((int)pe.type + 2);
            }
            typv = new_types;
        }
        else if(op->type_ == qop_type::group) {
            auto grp = std::dynamic_pointer_cast<group_op>(op);
            std::vector<int> new_types;
            for(auto g : grp->grpkey_pos_) {
                new_types.push_back(typv[g]);
            }
            typv = new_types;
        } else if(op->type_ == qop_type::aggr) {
            auto aggr = std::dynamic_pointer_cast<aggr_op>(op);
            for(auto a : aggr->aggrs_) {
                if((a.first.compare("count") == 0) || (a.first.compare("sum") == 0) ) {
                    typv.push_back(2);
                } else {
                    typv.push_back(3);
                }
            }
        }
        else if(op->type_ == qop_type::cross_join) {
            auto jop = std::dynamic_pointer_cast<join_op>(op);
            get_rhs_type(jop->inputs_[1], typv);
        } else if(op->type_ == qop_type::left_join) {
            auto jop = std::dynamic_pointer_cast<join_op>(op);
            get_rhs_type(jop->inputs_[1], typv);
        } else if(op->type_ == qop_type::nest_loop_join) {
            auto jop = std::dynamic_pointer_cast<join_op>(op);
            get_rhs_type(jop->inputs_[1], typv);
        } else if(op->type_ == qop_type::hash_join) {
            auto jop = std::dynamic_pointer_cast<join_op>(op);
            get_rhs_type(jop->inputs_[1], typv);
        }
        op = op->inputs_[0];
    }
}

/**
 * Generate code for the join operator
 */
void codegen_inline_visitor::visit(std::shared_ptr<join_op> op) {
    pipelined = true;
    jids.push_back(query_id_inline);
    
    // obtain types of rhs
    std::vector<int> types;
    get_rhs_type(op->inputs_[1], types);

    op->name_ = "";

    // create all basic blocks for the join processing
    BasicBlock *join_lhs_entry = BasicBlock::Create(ctx.getContext(), "entry_join_lhs", main_function);
    BasicBlock *left_outer = BasicBlock::Create(ctx.getContext(), "left_outer", main_function);
    BasicBlock *nested_loop = BasicBlock::Create(ctx.getContext(), "nested_loop", main_function);
    BasicBlock *nested_joinable = BasicBlock::Create(ctx.getContext(), "nested_loop_joinable", main_function);

    BasicBlock *hash_join = BasicBlock::Create(ctx.getContext(), "hash_join_probe", main_function);
    BasicBlock *hj_body = BasicBlock::Create(ctx.getContext(), "hj_body", main_function);
    BasicBlock *hj_joinable = BasicBlock::Create(ctx.getContext(), "hj_joinable", main_function);
    BasicBlock *for_each_next = BasicBlock::Create(ctx.getContext(), "for_each_next_rship", main_function);
    BasicBlock *concat_qrl = BasicBlock::Create(ctx.getContext(), "concat_qrl", main_function);
    BasicBlock *incr_loop = BasicBlock::Create(ctx.getContext(), "incr_loop", main_function);
    BasicBlock *undang = BasicBlock::Create(ctx.getContext(), "undang", main_function);
    BasicBlock *return_handle = BasicBlock::Create(ctx.getContext(), "handle_ret", main_function);
    BasicBlock *consume = BasicBlock::Create(ctx.getContext(), "consume", main_function);
    BasicBlock *end = BasicBlock::Create(ctx.getContext(), "end", main_function);


    // obtain all FunctionCallees
    auto get_join_tp_at = ctx.extern_func("get_join_tp_at");
    auto node_reg = ctx.extern_func("get_node_res_at");
    auto rship_reg = ctx.extern_func("get_rship_res_at");
    auto rship_by_id = ctx.extern_func("rship_by_id");
    auto get_size = ctx.extern_func("get_mat_res_size");
    auto insert_join_id_input = ctx.extern_func("insert_join_id_input");
    auto get_join_id_at = ctx.extern_func("get_join_id_at");

    auto fadd_now = ctx.extern_func("get_now");
    auto fadd_time_diff = ctx.extern_func("add_time_diff");

    auto int_to_reg = ctx.extern_func("int_to_reg");

    // link with previous operator
    ctx.getBuilder().SetInsertPoint(prev_bb);
    ctx.getBuilder().CreateBr(join_lhs_entry);
    ctx.getBuilder().SetInsertPoint(join_lhs_entry);

    Value* t_start = nullptr;
    Value* t_end = nullptr;

    if(profiling) 
        t_start = ctx.getBuilder().CreateCall(fadd_now, {});
    cur_idx = ctx.getBuilder().CreateAlloca(ctx.int64Ty);

    // init idx and dangling flag
    ctx.getBuilder().CreateStore(ctx.LLVM_ONE, dangling);
    ctx.getBuilder().CreateStore(ctx.LLVM_ZERO, cur_idx);

    Value *tp = nullptr;

    // get the join identifier and the appropriate rhs tuple list
    auto jid = ConstantInt::get(ctx.int64Ty, query_id_inline);
    auto ma = ctx.getBuilder().CreateCall(get_size, {jid});
    ctx.getBuilder().CreateStore(ma, max_idx);
    query_id_inline++;

    //Value *rhs_id;
    //auto rhs_id_alloc = insert_alloca(ctx.int64Ty);
    auto rhs_id_alloc = insert_alloca(ctx.int64Ty);

    BasicBlock *loop_body = nullptr;
    
    if(op->jop_ == JOIN_OP::NESTED_LOOP) {
            loop_body = ctx.while_loop_condition(main_function, cur_idx, max_idx, PContext::WHILE_COND::LT, end,
                                            [&](BasicBlock *body, BasicBlock *epilog) {
                                                auto pos = ctx.getBuilder().CreateLoad(cur_idx);
                                                auto rhs_id = ctx.getBuilder().CreateCall(get_join_id_at, {jid, pos});
                                                
                                                tp = ctx.getBuilder().CreateCall(get_join_tp_at, {jid, pos});
                                                ctx.getBuilder().CreateStore(rhs_id, rhs_id_alloc);
                                                ctx.getBuilder().CreateBr(nested_loop); 
                                            });  
    } else if(op->jop_ == JOIN_OP::HASH_JOIN) {
        ctx.getBuilder().CreateBr(hash_join);
    } else {
        // iterate through the join list
        loop_body = ctx.while_loop_condition(main_function, cur_idx, max_idx, PContext::WHILE_COND::LT, end,
                                                [&](BasicBlock *body, BasicBlock *epilog) {
                                                    // get current index for join vector
                                                    auto idx = ctx.getBuilder().CreateLoad(cur_idx);
                                                    tp = ctx.getBuilder().CreateCall(get_join_tp_at, {jid, idx});

                                                    if(op->jop_ == JOIN_OP::CROSS) {
                                                        // process cross join
                                                        ctx.getBuilder().CreateBr(concat_qrl);
                                                    } else if(op->jop_ == JOIN_OP::LEFT_OUTER) {
                                                        // process cross join
                                                        ctx.getBuilder().CreateBr(left_outer);
                                                    }
                                                });        
    }

    BasicBlock *loop_rship = nullptr;
    if(op->jop_ == JOIN_OP::LEFT_OUTER) {
        // for left outer join
        ctx.getBuilder().SetInsertPoint(left_outer);

        // get lhs tuple at id -> direct register value
        auto lhs = reg_query_results.at(op->join_pos_.first).reg_val;

        // rhs is materialized, call extern function to obtain tuple at idx
        auto idx = ConstantInt::get(ctx.int64Ty, op->join_pos_.second);
        auto rhs = ctx.getBuilder().CreateCall(node_reg, {tp, idx});

        // get the appropriate fields to compare, rship ids
        auto rhs_id = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(rhs, 1));
        auto node_rship_id = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(lhs, 2));
        ctx.getBuilder().CreateStore(node_rship_id, cross_join_idx.back());

        // create space for storage
        auto ra = ctx.getBuilder().CreateAlloca(ctx.rshipPtrTy);
        auto ridx = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
        ctx.getBuilder().CreateStore(node_rship_id, ridx);

        //Value *rship; //TODO: fix workaround for LLVM 11 bug 

        // iterate through relationship list of lhs node
        loop_rship = ctx.while_loop_condition(main_function, ridx, rhs_alloca, PContext::WHILE_COND::LT, incr_loop,
                                                  [&](BasicBlock *, BasicBlock *) {
                                                      auto cur_id = ctx.getBuilder().CreateLoad(ridx);

                                                      // get the current rship
                                                      auto rship = ctx.getBuilder().CreateCall(rship_by_id, {gdb, cur_id});
                                                      ctx.getBuilder().CreateStore(rship, ra);
                                                      // get the dest node of the rship
                                                      auto to_node = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(rship, 3));
                                                      
                                                      // check if the dest node is the rhs node
                                                      auto concat_cond = ctx.getBuilder().CreateICmpEQ(rhs_id, to_node);

                                                      // handle dangling node
                                                      ctx.getBuilder().CreateCondBr(concat_cond, undang, for_each_next);
                                                  });

        // get next rship from list
        ctx.getBuilder().SetInsertPoint(for_each_next);
        {
            auto rship = ctx.getBuilder().CreateLoad(ra);
            auto next_rship = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(rship, 4));
            ctx.getBuilder().CreateStore(next_rship, ridx);
            ctx.getBuilder().CreateBr(loop_rship);
        }

        // handle danling flag
        ctx.getBuilder().SetInsertPoint(undang);
        ctx.getBuilder().CreateStore(ctx.LLVM_ZERO, dangling);
        ctx.getBuilder().CreateBr(concat_qrl);

        // handling backflow
        ctx.getBuilder().SetInsertPoint(return_handle);
        auto lh = ctx.getBuilder().CreateLoad(ridx);
        auto is_reached = ctx.getBuilder().CreateICmpULT(lh, ctx.UNKNOWN_ID);
        ctx.getBuilder().CreateCondBr(is_reached, for_each_next, loop_body);

    } else if(op->jop_ == JOIN_OP::NESTED_LOOP) {
        // for nested loop join
        ctx.getBuilder().SetInsertPoint(nested_loop);
        //ctx.getBuilder().CreateBr(main_return);
        
        // get lhs tuple at id -> direct register value
        auto lhs = reg_query_results.at(op->join_pos_.first).reg_val;
        // extract the lhs id with GEP instruction
        auto lhs_id = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(lhs, 1));
        
        auto rhs_id = ctx.getBuilder().CreateLoad(rhs_id_alloc);
        auto id_eq = ctx.getBuilder().CreateICmpEQ(lhs_id, rhs_id);
        ctx.getBuilder().CreateCondBr(id_eq, nested_joinable, incr_loop);
        
        ctx.getBuilder().SetInsertPoint(nested_joinable);
        ctx.getBuilder().CreateBr(concat_qrl);

    } else if(op->jop_ == JOIN_OP::HASH_JOIN) {
        ctx.getBuilder().SetInsertPoint(hash_join);
        
        // get lhs tuple at id -> direct register value
        auto lhs = reg_query_results.at(op->join_pos_.first).reg_val;
        // extract the lhs id with GEP instruction
        auto lhs_id = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(lhs, 1));

        // calculate the bucket id 
        auto bucket_size = ConstantInt::get(ctx.int64Ty, 10);
        auto bucket_id = ctx.getBuilder().CreateSRem(lhs_id, bucket_size);

        auto get_hj_input_size = ctx.extern_func("get_hj_input_size");
        auto get_hj_input_id = ctx.extern_func("get_hj_input_id");
        auto get_query_result = ctx.extern_func("get_query_result");

        loop_body = BasicBlock::Create(ctx.getContext(), "hj_head", main_function);

        auto input_size = ctx.getBuilder().CreateCall(get_hj_input_size, {jid, bucket_id});
        auto cur_idx = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
        ctx.getBuilder().CreateStore(ctx.LLVM_ZERO, cur_idx);
        ctx.getBuilder().CreateBr(loop_body);

        ctx.getBuilder().SetInsertPoint(loop_body);
        auto idx = ctx.getBuilder().CreateLoad(cur_idx);
        auto hj_lt = ctx.getBuilder().CreateICmpULT(idx, input_size);
        ctx.getBuilder().CreateCondBr(hj_lt, hj_body, end);

        ctx.getBuilder().SetInsertPoint(hj_body);
        auto id = ctx.getBuilder().CreateCall(get_hj_input_id, {jid, bucket_id, idx});
        auto incr_hj_id = ctx.getBuilder().CreateAdd(idx, ctx.LLVM_ONE);
        ctx.getBuilder().CreateStore(incr_hj_id, cur_idx);
        auto hj_eq = ctx.getBuilder().CreateICmpEQ(lhs_id, id);
        ctx.getBuilder().CreateCondBr(hj_eq, hj_joinable, loop_body);

        ctx.getBuilder().SetInsertPoint(hj_joinable);
        tp = ctx.getBuilder().CreateCall(get_query_result, {jid, bucket_id, idx});
        ctx.getBuilder().CreateBr(concat_qrl);
    }

    // increment the outer loop counter for next query result
    ctx.getBuilder().SetInsertPoint(incr_loop);
    {
        auto i = ctx.getBuilder().CreateLoad(cur_idx);
        ctx.getBuilder().CreateStore(ctx.getBuilder().CreateAdd(i, ctx.LLVM_ONE), cur_idx);

        if(op->jop_ == JOIN_OP::LEFT_OUTER) {
            auto dang = ctx.getBuilder().CreateLoad(dangling);
            auto is_dangling = ctx.getBuilder().CreateICmpEQ(dang, ctx.LLVM_ONE);
            ctx.getBuilder().CreateCondBr(is_dangling, concat_qrl, loop_body);
        } else {
            ctx.getBuilder().CreateBr(loop_body);
        }
    }
    
    // merge the lhs and rhs    
    ctx.getBuilder().SetInsertPoint(concat_qrl);

    for(auto i = 0u; i < types.size(); i++) {
        auto idx = ConstantInt::get(ctx.int64Ty, i);
        if(types.at(i) == 0) {
            auto n = ctx.getBuilder().CreateCall(node_reg, {tp, idx});
            reg_query_results.push_back({n, 0});
        } else if(types.at(i) == 1) {
            auto r = ctx.getBuilder().CreateCall(rship_reg, {tp, idx});
            reg_query_results.push_back({r, 1});
        } else if(types.at(i) == 2) {
            auto i = ctx.getBuilder().CreateCall(int_to_reg, {tp, idx});
            auto i_alloc = insert_alloca(ctx.int64Ty);
            ctx.getBuilder().CreateStore(i, i_alloc);
            reg_query_results.push_back({i_alloc, 2});
        } 
    }
    // add the dangling flag to the tuple result
    if(op->jop_ == JOIN_OP::LEFT_OUTER)
        reg_query_results.push_back({dangling, 7});

    if(op->jop_ == JOIN_OP::CROSS || op->jop_ == JOIN_OP::NESTED_LOOP) {
        auto i = ctx.getBuilder().CreateLoad(cur_idx);
        auto i_add = ctx.getBuilder().CreateAdd(i, ctx.LLVM_ONE);
        ctx.getBuilder().CreateStore(i_add, cur_idx);
    }

    ctx.getBuilder().CreateBr(consume);

    ctx.getBuilder().SetInsertPoint(consume);

    if(profiling) {
        t_end = ctx.getBuilder().CreateCall(fadd_now, {});
        ctx.getBuilder().CreateCall(fadd_time_diff, {query_context, ConstantInt::get(ctx.int64Ty, op->op_id_), t_start, t_end});
    }
    prev_bb = consume;

    ctx.getBuilder().SetInsertPoint(end);
    {
        ctx.getBuilder().CreateBr(main_return);
    }

    if(op->jop_ == JOIN_OP::CROSS || op->jop_ == JOIN_OP::NESTED_LOOP || op->jop_ == JOIN_OP::HASH_JOIN)
        main_return = loop_body;
    else if(op->jop_ == JOIN_OP::LEFT_OUTER)
        main_return = return_handle;
}

/**
 * Generate code for the sort operator
 * For now it execute the given sorting predicate
 */ 
void codegen_inline_visitor::visit(std::shared_ptr<sort_op> op) {
    op->name_ = "sort_finish_"+qid_;
    Function *df_finish = Function::Create(ctx.finishFctTy, Function::ExternalLinkage, op->name_,
                                           ctx.getModule());

    BasicBlock *sort_entry = BasicBlock::Create(ctx.getContext(), "entry", main_finish);

    auto fadd_now = ctx.extern_func("get_now");
    auto fadd_time_diff = ctx.extern_func("add_time_diff");
    ctx.getBuilder().SetInsertPoint(df_finish_bb);
    ctx.getBuilder().CreateBr(sort_entry);
    ctx.getBuilder().SetInsertPoint(sort_entry);

    Value* t_start = nullptr;
    Value* t_end = nullptr;

    if(profiling) 
        t_start = ctx.getBuilder().CreateCall(fadd_now, {});

    auto res = df_finish->args().begin();

    auto sort_fc_raw = ConstantInt::get(ctx.int64Ty, (int64_t)sort_op::sort);
    auto sort_fc_ptr = ctx.getBuilder().CreateIntToPtr(sort_fc_raw, ctx.int64PtrTy);
    auto sort_fc = ctx.getBuilder().CreateBitCast(sort_fc_ptr, FunctionType::get(ctx.voidTy, {ctx.int64PtrTy}, false)->getPointerTo());

    ctx.getBuilder().CreateCall(FunctionType::get(ctx.voidTy, {ctx.int64PtrTy}, false), sort_fc, {main_finish->arg_begin()});

    if(profiling) {
        t_end = ctx.getBuilder().CreateCall(fadd_now, {});
        ctx.getBuilder().CreateCall(fadd_time_diff, {query_context, ConstantInt::get(ctx.int64Ty, op->op_id_), t_start, t_end});
    }
    df_finish_bb = sort_entry;
    //ctx.getBuilder().CreateRetVoid();
}

/**
 * Generates code for the limit operator
 */ 
void codegen_inline_visitor::visit(std::shared_ptr<limit_op> op) {
    op->name_ = "";
    BasicBlock *limit = BasicBlock::Create(ctx.getContext(), "limit_entry", main_function);
    BasicBlock *limit_reached = BasicBlock::Create(ctx.getContext(), "limit_entry", main_function);
    BasicBlock *limit_cont = BasicBlock::Create(ctx.getContext(), "limit_entry", main_function);

    auto fadd_now = ctx.extern_func("get_now");
    auto fadd_time_diff = ctx.extern_func("add_time_diff");

    // link with previous operator
    ctx.getBuilder().SetInsertPoint(prev_bb);
    ctx.getBuilder().CreateBr(limit);
    ctx.getBuilder().SetInsertPoint(limit);

    Value* t_start = nullptr;
    Value* t_end = nullptr;

    if(profiling) 
        t_start = ctx.getBuilder().CreateCall(fadd_now, {});

    // get the given limit
    auto res_limit = ConstantInt::get(ctx.int64Ty, op->limit_);

    // load the value from the global result counter
    auto cur_cnt = ctx.getBuilder().CreateLoad(res_counter);

    // check if the limit is reached
    auto reached = ctx.getBuilder().CreateICmpEQ(cur_cnt, res_limit);
    ctx.getBuilder().CreateCondBr(reached, limit_reached, limit_cont);

    // if the limit is reached branch to the global end block
    ctx.getBuilder().SetInsertPoint(limit_reached);

    if(profiling) {
        t_end = ctx.getBuilder().CreateCall(fadd_now, {});
        ctx.getBuilder().CreateCall(fadd_time_diff, {query_context, ConstantInt::get(ctx.int64Ty, op->op_id_), t_start, t_end});
    }

    ctx.getBuilder().CreateBr(scan_nodes_end);


    // increment the global result counter
    ctx.getBuilder().SetInsertPoint(limit_cont);
    auto incr = ctx.getBuilder().CreateAdd(cur_cnt, ctx.LLVM_ONE);
    ctx.getBuilder().CreateStore(incr, res_counter);
    if(profiling) {
        t_end = ctx.getBuilder().CreateCall(fadd_now, {});
        ctx.getBuilder().CreateCall(fadd_time_diff, {query_context, ConstantInt::get(ctx.int64Ty, op->op_id_), t_start, t_end});
    }
    prev_bb = limit_cont;

}

/**
 * Generates code for the materialization of the rhs of the join
 */
void codegen_inline_visitor::visit(std::shared_ptr<end_op> op) {
    op->name_ = "";
    //auto join_insert_left = ctx.extern_func("join_insert_left");

    auto obtain_mat_tuple = ctx.extern_func("obtain_mat_tuple");
    auto mat_node = ctx.extern_func("mat_node");
    auto mat_rship = ctx.extern_func("mat_rship");
    auto collect_tuple_join = ctx.extern_func("collect_tuple_join");
    auto collect_tuple_hash_join = ctx.extern_func("collect_tuple_hash_join");
    auto insert_join_id_input = ctx.extern_func("insert_join_id_input");
    auto insert_join_bucket_input = ctx.extern_func("insert_join_bucket_input");

    auto mat_reg = ctx.extern_func("mat_reg_value");
    auto fadd_now = ctx.extern_func("get_now");
    auto fadd_time_diff = ctx.extern_func("add_time_diff");

    //FunctionCallee mat_reg = ctx.extern_func("mat_reg_value");
    BasicBlock *bb = BasicBlock::Create(ctx.getContext(), "entry_join_rhs", main_function);

    ctx.getBuilder().SetInsertPoint(prev_bb);
    ctx.getBuilder().CreateBr(bb);
    ctx.getBuilder().SetInsertPoint(bb);

    Value* t_start = nullptr;
    Value* t_end = nullptr;

    if(profiling) 
        t_start = ctx.getBuilder().CreateCall(fadd_now, {});

    // collect materialized tuple -> joiner rhs_input @ join id
    auto last_jid = jids.front();
    jids.pop_front();
    auto jid = ConstantInt::get(ctx.int64Ty, last_jid);
    
    Value *remainder = nullptr;

    if(op->join_op_ == JOIN_OP::NESTED_LOOP) {
        auto n = reg_query_results[op->qr_pos_].reg_val;
        auto id = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(n, 1));
        ctx.getBuilder().CreateCall(insert_join_id_input, {jid, id});
    } else if(op->join_op_ == JOIN_OP::HASH_JOIN) {
        auto n = reg_query_results[op->qr_pos_].reg_val;
        auto id = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(n, 1));
        auto num_bucket = ConstantInt::get(ctx.int64Ty, 10);
        remainder = ctx.getBuilder().CreateSRem(id, num_bucket);
        ctx.getBuilder().CreateCall(insert_join_bucket_input, {jid, remainder, id});
    }
    // obtain tuple object for materialization -> joiner mat_tuple
    auto tp = ctx.getBuilder().CreateCall(obtain_mat_tuple, {});


    // materialize each result from registers
    for(auto & res : reg_query_results) {
        //auto type = ConstantInt::get(ctx.int64Ty, res.type);
        if(res.type == 0) {
            ctx.getBuilder().CreateCall(mat_node, {res.reg_val, tp});
        } else if(res.type == 1) {
            ctx.getBuilder().CreateCall(mat_rship, {res.reg_val, tp});
        } else if(res.type == 2) {
            auto cv_reg = ctx.getBuilder().CreateBitCast(res.reg_val, ctx.int64PtrTy);
            ctx.getBuilder().CreateCall(mat_reg, {gdb, cv_reg, ConstantInt::get(ctx.int64Ty, 92)});
        } else if(res.type == 3) {
            auto cv_reg = ctx.getBuilder().CreateBitCast(res.reg_val, ctx.int64PtrTy);
            ctx.getBuilder().CreateCall(mat_reg, {gdb, cv_reg, ConstantInt::get(ctx.int64Ty, 93)});
        }
    }

    if(op->join_op_ == JOIN_OP::HASH_JOIN) {
        ctx.getBuilder().CreateCall(collect_tuple_hash_join, {jid, remainder, tp});
    } else {
        ctx.getBuilder().CreateCall(collect_tuple_join, {jid, tp});
    }
    

    if(profiling) {
        t_end = ctx.getBuilder().CreateCall(fadd_now, {});
        ctx.getBuilder().CreateCall(fadd_time_diff, {query_context, ConstantInt::get(ctx.int64Ty, op->op_id_), t_start, t_end});
    }

    ctx.getBuilder().CreateBr(main_return);

    ctx.getBuilder().SetInsertPoint(df_finish_bb);
    ctx.getBuilder().CreateRetVoid();
}

/**
 * Generates code for the create operation
 */
void codegen_inline_visitor::visit(std::shared_ptr<create_op> op) {
    op->name_ = "";
    BasicBlock *bb;
    bool access = false;
    // if the create is a access path -> init function
    if(!main_function) {
        op->name_ = "query_start";
        access = true;
        Function *df_finish = Function::Create(ctx.finishFctTy, Function::ExternalLinkage, "default_finish_"+qid_,
                                               ctx.getModule());

        BasicBlock *df_finish_bb = BasicBlock::Create(ctx.getContext(), "entry", df_finish);
        ctx.getBuilder().SetInsertPoint(df_finish_bb);
        ctx.getBuilder().CreateRetVoid();

        main_function = Function::Create(ctx.startFctTy, Function::ExternalLinkage, op->name_, ctx.getModule());
        bb = BasicBlock::Create(ctx.getModule().getContext(), "entry", main_function);
        main_return = BasicBlock::Create(ctx.getModule().getContext(), "end", main_function);
        init_function(bb);
    } else { // else link with previous operator
        bb = BasicBlock::Create(ctx.getModule().getContext(), "entry_create", main_function);
        ctx.getBuilder().SetInsertPoint(prev_bb);
        ctx.getBuilder().CreateBr(bb);
        ctx.getBuilder().SetInsertPoint(bb);

    }
    auto consume = BasicBlock::Create(ctx.getModule().getContext(), "entry", main_function);

    gdb = main_function->args().begin();
    ty = main_function->args().begin() + 5;
    rs = main_function->args().begin() + 6;
    finish = main_function->args().begin() + 8;
    offset = main_function->args().begin() + 9;
    queryArgs = main_function->args().begin() + 10;

    //create node by calling external function
    if(op->produced_type_ == 0) {
        auto create_node = ctx.extern_func("create_node");
        auto opid = ConstantInt::get(ctx.int64Ty, op->op_id_);
        auto sec_opid = ConstantInt::get(ctx.int64Ty, op->op_id_+1);
        auto label = ctx.getBuilder().CreateBitCast(ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(queryArgs, {ctx.LLVM_ZERO, opid})), ctx.int8PtrTy);
        auto prop_ptr = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(queryArgs, {ctx.LLVM_ZERO, sec_opid}));
        auto node = ctx.getBuilder().CreateCall(create_node, {gdb, label, prop_ptr});
        reg_query_results.push_back({node, 0});
        ctx.getBuilder().CreateBr(consume);
    } else { // create rship
        auto create_rship = ctx.extern_func("create_rship");
        auto opid = ConstantInt::get(ctx.int64Ty, op->op_id_);
        auto sec_opid = ConstantInt::get(ctx.int64Ty, op->op_id_+1);
        auto label = ctx.getBuilder().CreateBitCast(ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(queryArgs, {ctx.LLVM_ZERO, opid})), ctx.int8PtrTy);
        auto prop_ptr = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(queryArgs, {ctx.LLVM_ZERO, sec_opid}));

        auto n1 = reg_query_results[op->src_des_.first].reg_val;
        auto n2 = reg_query_results[op->src_des_.second].reg_val;

        auto rship = ctx.getBuilder().CreateCall(create_rship, {gdb, label, n1, n2, prop_ptr});
        reg_query_results.push_back({rship, 1});

        ctx.getBuilder().CreateBr(consume);
    }


    ctx.getBuilder().SetInsertPoint(consume);
    prev_bb = consume;

    if(access) {
        ctx.getBuilder().SetInsertPoint(main_return);
        ctx.getBuilder().CreateRetVoid();
    }
}


/**
 * Generates code for the group_by operation
 */
void codegen_inline_visitor::visit(std::shared_ptr<group_op> op) {
    
    op->name_ = "";

    if(pipelined_finish) {
        //main_function = Function::Create(ctx.startFctTy, Function::ExternalLinkage, query_id_str, ctx.getModule());
        main_finish   = Function::Create(ctx.finishFctTy, Function::ExternalLinkage, "finish_"+query_id_str, ctx.getModule());
        
        //pipelines.push_back(std::string(main_function->getName()));
        pipelines.push_back(std::string(main_finish->getName()));
    }

    auto get_node_grpkey = ctx.extern_func("get_node_grpkey");
    auto get_rship_grpkey = ctx.extern_func("get_rship_grpkey");
    auto get_int_grpkey = ctx.extern_func("get_int_grpkey");
    auto get_string_grpkey = ctx.extern_func("get_string_grpkey");
    auto get_time_grpkey = ctx.extern_func("get_time_grpkey");
    auto add_to_group = ctx.extern_func("add_to_group");

    auto fadd_now = ctx.extern_func("get_now");
    auto fadd_time_diff = ctx.extern_func("add_time_diff");

    BasicBlock *group_entry = BasicBlock::Create(ctx.getContext(), "group_entry", cur_pipeline);

    ctx.getBuilder().SetInsertPoint(prev_bb);
    ctx.getBuilder().CreateBr(group_entry);
    ctx.getBuilder().SetInsertPoint(group_entry);

    Value* t_start = nullptr;
    Value* t_end = nullptr;

    if(profiling) 
        t_start = ctx.getBuilder().CreateCall(fadd_now, {});

    // materialize tuple
    FunctionCallee mat_reg = ctx.extern_func("mat_reg_value");
    auto qctx = cur_pipeline->args().begin();
    auto qarg = cur_pipeline->args().begin() + 1;
    auto g = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(qctx, 0));

    auto opid = ConstantInt::get(ctx.int64Ty, op->op_id_);
    auto arg_pos = ConstantInt::get(ctx.int64Ty, 3);
    //auto qargs = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(qctx, 3));
    
    auto grp_p = ctx.getBuilder().CreateBitCast(
        ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(qarg, {ctx.LLVM_ZERO, opid})),
        ctx.int8PtrTy);

    for(auto & res : reg_query_results) {
        // value type 7 => boolean, already transformed into integer
        auto type_id = (res.type == 7 ? 2 : res.type == 0 ? 90 : res.type == 1 ? 91 : res.type);
        auto type = ConstantInt::get(ctx.int64Ty, type_id);
        auto cv_reg = ctx.getBuilder().CreateBitCast(res.reg_val, ctx.int64PtrTy);
        
        ctx.getBuilder().CreateCall(mat_reg, {g, cv_reg, type});
        
    }

    std::vector<std::pair<int,int>> pos_type;
    int i = 0;
    for(auto pos : op->grpkey_pos_) {
        auto qr = reg_query_results[pos];
        auto pos_val = ConstantInt::get(ctx.int64Ty, pos);
        switch(qr.type) {
            case 0: {
                ctx.getBuilder().CreateCall(get_node_grpkey, {qr.reg_val, pos_val});
                
                break;
            } 
            case 1: {
                ctx.getBuilder().CreateCall(get_rship_grpkey, {qr.reg_val, pos_val});
                break;
            } 
            case 2: {
                ctx.getBuilder().CreateCall(get_int_grpkey, {ctx.getBuilder().CreateLoad(qr.reg_val), pos_val});
                break;
            } 
            case 3: {
                
                break;
            }
            case 4: {
                ctx.getBuilder().CreateCall(get_string_grpkey, {qr.reg_val, pos_val});
                break;
            }
            case 6: {
                ctx.getBuilder().CreateCall(get_time_grpkey, {qr.reg_val, pos_val});
                break;
            } 
            break;
        }
        pos_type.push_back({i++, qr.type});
    }

    ctx.getBuilder().CreateCall(add_to_group, {grp_p});
    
    if(profiling) {
        t_end = ctx.getBuilder().CreateCall(fadd_now, {});
        ctx.getBuilder().CreateCall(fadd_time_diff, {query_context, ConstantInt::get(ctx.int64Ty, op->op_id_), t_start, t_end});
    }

    ctx.getBuilder().CreateBr(main_return);

    //modify the finish processing -> add to last basic block of pipeline
    BasicBlock *group_finish = BasicBlock::Create(ctx.getContext(), "group_finish", main_finish);
    ctx.getBuilder().SetInsertPoint(df_finish_bb);

    if(pipelined_finish)
        //ctx.getBuilder().CreateBr(main_return);
        ctx.getBuilder().CreateRetVoid();
    else
        ctx.getBuilder().CreateBr(group_finish);
    
    prev_bb = group_entry;
    
    ctx.getBuilder().SetInsertPoint(group_finish);
    auto qctx_f = main_finish->args().begin() + 1;
    auto g_f = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(qctx_f, 0));
    auto grp_pf = ctx.getBuilder().CreateBitCast(
        ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(qctx_f, {ctx.LLVM_ZERO, opid})),
        ctx.int8PtrTy);

    auto finish_group_by = ctx.extern_func("finish_group_by");
    auto rs_arg = main_finish->args().begin()+2;
    ctx.getBuilder().CreateCall(finish_group_by, {grp_pf, rs_arg});

    auto int_to_reg = ctx.extern_func("int_to_reg");
    auto str_to_reg = ctx.extern_func("str_to_reg");
    auto node_to_reg = ctx.extern_func("node_to_reg");
    auto rship_to_reg = ctx.extern_func("rship_to_reg");
    auto time_to_reg = ctx.extern_func("time_to_reg");
    auto get_grp_rs_count = ctx.extern_func("get_grp_rs_count");
    auto grp_demat_at = ctx.extern_func("grp_demat_at");

    auto demat_results = ctx.getBuilder().CreateCall(get_grp_rs_count, {grp_pf});
    //auto cur_pos = insert_alloca(ctx.int64Ty);
    auto cur_pos = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    ctx.getBuilder().CreateStore(ctx.LLVM_ZERO, cur_pos);
    
    BasicBlock *group_loop_head = BasicBlock::Create(ctx.getContext(), "group_loop_head", main_finish);
    BasicBlock *group_loop_body = BasicBlock::Create(ctx.getContext(), "group_loop_body", main_finish);
    BasicBlock *group_loop_exit = BasicBlock::Create(ctx.getContext(), "group_loop_exit", main_finish);

    // demat all grouped intermediate result again to registers
    ctx.getBuilder().CreateBr(group_loop_head);

    ctx.getBuilder().SetInsertPoint(group_loop_head);
    auto cur_idx = ctx.getBuilder().CreateLoad(cur_pos);
    auto cmp_head = ctx.getBuilder().CreateICmpULT(cur_idx, demat_results);
    ctx.getBuilder().CreateCondBr(cmp_head, group_loop_body, group_loop_exit);

    ctx.getBuilder().SetInsertPoint(group_loop_body);
    auto tuple = ctx.getBuilder().CreateCall(grp_demat_at, {grp_pf, cur_idx});
    auto new_idx = ctx.getBuilder().CreateAdd(cur_idx, ctx.LLVM_ONE);
    ctx.getBuilder().CreateStore(new_idx, cur_pos);

    std::vector<QR_VALUE> new_query_res;
    for(auto & pt : pos_type) {
        auto pos = ConstantInt::get(ctx.int64Ty, pt.first);
        Value *demat = nullptr;
        switch(pt.second) {
            case 0: {
                auto n = ctx.getBuilder().CreateCall(node_to_reg, {tuple, pos});
                new_query_res.push_back({n, pt.second});
                continue;
            }
            case 1: {
                auto r = ctx.getBuilder().CreateCall(rship_to_reg, {tuple, pos});
                new_query_res.push_back({r, pt.second});
                continue;
            }
            case 2: {
                auto i = ctx.getBuilder().CreateCall(int_to_reg, {tuple, pos});
                //demat = insert_alloca(ctx.int64Ty);
                demat = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
                ctx.getBuilder().CreateStore(i, demat);
                break;
            }
            case 3: continue;
            case 4: {
                auto str = ctx.getBuilder().CreateCall(str_to_reg, {tuple, pos});
                //demat = insert_alloca(ctx.int64Ty);
                demat = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
                ctx.getBuilder().CreateStore(str, demat);
                break;
            }
            case 6: {
                auto t = ctx.getBuilder().CreateCall(time_to_reg, {tuple, pos});
                //demat = insert_alloca(ctx.int64Ty);
                demat = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
                ctx.getBuilder().CreateStore(t, demat);
                break;
            }
        }
        new_query_res.push_back({demat, pt.second});
    }
    reg_query_results = new_query_res;
    //ctx.getBuilder().CreateBr(group_loop_head);

    ctx.getBuilder().SetInsertPoint(group_loop_exit);
  //  if(pipelined_finish)
  //      ctx.getBuilder().CreateBr(main_return);
    df_finish_bb = group_loop_exit;
    main_return = group_loop_head;
    prev_bb = group_loop_body;

    pipelined_finish = true;
}

/**
 * Generates code for aggr operations
 */
void codegen_inline_visitor::visit(std::shared_ptr<aggr_op> op) {
    op->name_ = "";
    auto init_grp_aggr = ctx.extern_func("init_grp_aggr");
    auto grp_cnt = ctx.extern_func("get_group_cnt");
    auto grp_total_cnt = ctx.extern_func("get_total_group_cnt");
    auto get_group_sum_int = ctx.extern_func("get_group_sum_int"); 
    auto get_group_sum_double = ctx.extern_func("get_group_sum_double");
    auto get_group_sum_uint = ctx.extern_func("get_group_sum_uint");  

    auto fadd_now = ctx.extern_func("get_now");
    auto fadd_time_diff = ctx.extern_func("add_time_diff");

    //modify the finish processing -> add to last basic block of pipeline
    BasicBlock *aggr_finish = BasicBlock::Create(ctx.getContext(), "aggr_finish", main_finish);
    ctx.getBuilder().SetInsertPoint(prev_bb);
    ctx.getBuilder().CreateBr(aggr_finish);
    ctx.getBuilder().SetInsertPoint(aggr_finish);


    auto opid = ConstantInt::get(ctx.int64Ty, op->op_id_);
    auto qctx_f = main_finish->args().begin() + 1;
    auto g_f = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(qctx_f, 0));

    auto grp_pf = ctx.getBuilder().CreateBitCast(
        ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(qctx_f, {ctx.LLVM_ZERO, opid})),
        ctx.int8PtrTy);
    
    Value* t_start = nullptr;
    Value* t_end = nullptr;

    if(profiling) 
        t_start = ctx.getBuilder().CreateCall(fadd_now, {});

    ctx.getBuilder().CreateCall(init_grp_aggr, {grp_pf});
    for(auto &aggr: op->aggrs_) {
        if(aggr.first.compare("count") == 0) {
            //auto cnt_alloc = insert_alloca(ctx.int64Ty);
            auto cnt_alloc =  ctx.getBuilder().CreateAlloca(ctx.int64Ty);
            auto cnt = ctx.getBuilder().CreateCall(grp_cnt, {grp_pf});
            ctx.getBuilder().CreateStore(cnt, cnt_alloc);
            reg_query_results.push_back({cnt_alloc, 2});
        } else if(aggr.first.compare("pcount") == 0) {
            //auto pcnt_alloc = insert_alloca(ctx.doubleTy);
            auto pcnt_alloc = ctx.getBuilder().CreateAlloca(ctx.doubleTy);
            auto cnt = ctx.getBuilder().CreateBitCast(ctx.getBuilder().CreateCall(grp_cnt, {grp_pf}), ctx.doubleTy);
            auto total_cnt = ctx.getBuilder().CreateBitCast(ctx.getBuilder().CreateCall(grp_total_cnt, {grp_pf}), ctx.doubleTy);
            auto cnt_div = ctx.getBuilder().CreateFDiv(cnt, total_cnt);
            auto pcount = ctx.getBuilder().CreateFMul(cnt_div, ConstantFP::get(ctx.getContext(), APFloat(100.0)));
            ctx.getBuilder().CreateStore(pcount, pcnt_alloc);
            reg_query_results.push_back({ctx.getBuilder().CreateBitCast(pcnt_alloc, ctx.int64PtrTy), 3});
        } else if(aggr.first.compare("sum") == 0) {
            auto pos = ConstantInt::get(ctx.int64Ty, aggr.second);
            switch(reg_query_results[aggr.second].type) {
                case 2: {
                    auto sum_alloc = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
                    //auto sum_alloc = insert_alloca(ctx.int64Ty);
                    auto sum = ctx.getBuilder().CreateCall(get_group_sum_int, {grp_pf, pos});
                    ctx.getBuilder().CreateStore(sum, sum_alloc);
                    reg_query_results.push_back({sum_alloc, 2});
                    break;
                }
                case 3: {
                    auto sum_alloc = insert_alloca(ctx.doubleTy);
                    auto sum = ctx.getBuilder().CreateCall(get_group_sum_double, {grp_pf, pos});
                    ctx.getBuilder().CreateStore(sum, sum_alloc);
                    reg_query_results.push_back({sum_alloc, 3});
                    break;
                }
                case 5: {
                    auto sum_alloc = insert_alloca(ctx.int64Ty);
                    auto sum = ctx.getBuilder().CreateCall(get_group_sum_uint, {grp_pf, pos});
                    ctx.getBuilder().CreateStore(sum, sum_alloc);
                    reg_query_results.push_back({sum_alloc, 5});
                    break;
                }
            }

        } else if(aggr.first.compare("avg") == 0) {
            auto pos = ConstantInt::get(ctx.int64Ty, aggr.second);
            switch(reg_query_results[aggr.second].type) {
                case 2: {
                    auto avg_alloc = ctx.getBuilder().CreateAlloca(ctx.doubleTy);
                    //auto avg_alloc = insert_alloca(ctx.doubleTy);
                    auto cnt = ctx.getBuilder().CreateBitCast(ctx.getBuilder().CreateCall(grp_cnt, {grp_pf}), ctx.doubleTy);
                    auto sum = ctx.getBuilder().CreateBitCast(ctx.getBuilder().CreateCall(get_group_sum_int, {grp_pf, pos}), ctx.doubleTy);
                    auto avg = ctx.getBuilder().CreateFDiv(sum, cnt);
                    ctx.getBuilder().CreateStore(avg, avg_alloc);
                    reg_query_results.push_back({avg_alloc, 3});
                    break;
                }
                case 3: {
                    auto avg_alloc = insert_alloca(ctx.doubleTy);
                    auto cnt = ctx.getBuilder().CreateBitCast(ctx.getBuilder().CreateCall(grp_cnt, {grp_pf}), ctx.doubleTy);
                    auto sum = ctx.getBuilder().CreateCall(get_group_sum_double, {grp_pf, pos});
                    auto avg = ctx.getBuilder().CreateUDiv(sum, cnt);
                    ctx.getBuilder().CreateStore(avg, avg_alloc);
                    reg_query_results.push_back({avg_alloc, 3});
                    break;
                }
                case 5: {
                    auto avg_alloc = insert_alloca(ctx.int64Ty);
                    auto cnt = ctx.getBuilder().CreateCall(grp_cnt, {grp_pf});
                    auto sum = ctx.getBuilder().CreateCall(get_group_sum_uint, {grp_pf, pos});
                    auto avg = ctx.getBuilder().CreateUDiv(sum, cnt);
                    ctx.getBuilder().CreateStore(avg, avg_alloc);
                    reg_query_results.push_back({avg_alloc, 5});
                    break;
                }
            }
        }
    }
    
    if(profiling) {
        t_end = ctx.getBuilder().CreateCall(fadd_now, {});
        ctx.getBuilder().CreateCall(fadd_time_diff, {query_context, ConstantInt::get(ctx.int64Ty, op->op_id_), t_start, t_end});
    }

    prev_bb = aggr_finish;
    //df_finish = aggr_finish;
    //main_function = main_finish;
    cur_pipeline = main_finish;
}

void codegen_inline_visitor::visit(std::shared_ptr<connected_op> op) {
    op->name_ = "";
    auto rship_by_id = ctx.extern_func("rship_by_id");
    auto fadd_now = ctx.extern_func("get_now");
    auto fadd_time_diff = ctx.extern_func("add_time_diff");

    BasicBlock *connected_entry = BasicBlock::Create(ctx.getContext(), "connected_entry", main_function);
    BasicBlock *connected_head = BasicBlock::Create(ctx.getContext(), "connected_head", main_function);
    BasicBlock *connected_body = BasicBlock::Create(ctx.getContext(), "connected_body", main_function);
    BasicBlock *connected_exit = BasicBlock::Create(ctx.getContext(), "connected_exit", main_function);
    BasicBlock *consume = BasicBlock::Create(ctx.getContext(), "consume", main_function);
    //BasicBlock *append_null = BasicBlock::Create(ctx.getContext(), "consume", main_function);

    ctx.getBuilder().SetInsertPoint(prev_bb);
    ctx.getBuilder().CreateBr(connected_entry);
    ctx.getBuilder().SetInsertPoint(connected_entry);

    Value* t_start = nullptr;
    Value* t_end = nullptr;

    if(profiling) 
        t_start = ctx.getBuilder().CreateCall(fadd_now, {});

    auto found = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    ctx.getBuilder().CreateStore(ctx.LLVM_ZERO, found);

    // get the src and dest node
    auto src = reg_query_results[op->src_des_.first].reg_val;
    auto dst = reg_query_results[op->src_des_.second].reg_val;

    // get the first from rship
    auto rship_id = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(src, 2));
    auto id_alloc = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    ctx.getBuilder().CreateStore(rship_id, id_alloc);

    // get the id of the dst node
    auto dst_id = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(dst, 1));
    ctx.getBuilder().CreateBr(connected_head);

    //iterate through rship list of src and find a dst with id dst_id
    ctx.getBuilder().SetInsertPoint(connected_head);
    auto unknown_id = ctx.getBuilder().CreateICmpEQ(dst_id, ctx.UNKNOWN_ID);
    ctx.getBuilder().CreateCondBr(unknown_id, connected_exit, connected_body);

    ctx.getBuilder().SetInsertPoint(connected_body);
    auto id = ctx.getBuilder().CreateLoad(id_alloc);
    auto rship = ctx.getBuilder().CreateCall(rship_by_id, {id});
    auto to_node = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(rship, 3));
    auto next_rship = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(rship, 4));
    ctx.getBuilder().CreateStore(next_rship, id_alloc);
    auto connected = ctx.getBuilder().CreateICmpEQ(to_node, dst_id);
    ctx.getBuilder().CreateCondBr(connected, consume, connected_head);

    ctx.getBuilder().SetInsertPoint(connected_exit);
    if(op->append_null_) {
        ctx.getBuilder().CreateBr(consume);
    } else {
        ctx.getBuilder().CreateBr(main_return);
    }

    if(op->append_null_) {
        reg_query_results.push_back({found, 2});
    }

    ctx.getBuilder().SetInsertPoint(consume);

    if(profiling) {
        t_end = ctx.getBuilder().CreateCall(fadd_now, {});
        ctx.getBuilder().CreateCall(fadd_time_diff, {query_context, ConstantInt::get(ctx.int64Ty, op->op_id_), t_start, t_end});
    }

    ctx.getBuilder().CreateStore(ctx.LLVM_ONE, consume);

    prev_bb = consume;
    main_return = connected_head;
}

void codegen_inline_visitor::visit(std::shared_ptr<append_op> op) {
    op->name_ = "";

    auto append_to_tuple = ctx.extern_func("append_to_tuple");
    auto get_qr_tuple = ctx.extern_func("get_qr_tuple");
    auto fadd_now = ctx.extern_func("get_now");
    auto fadd_time_diff = ctx.extern_func("add_time_diff");
    // insert the append processing before the tuples get materialized
    ctx.getBuilder().SetInsertPoint(pre_tuple_mat);

    Value* t_start = nullptr;
    Value* t_end = nullptr;

    if(profiling) 
        t_start = ctx.getBuilder().CreateCall(fadd_now, {});

    auto fct_raw = ConstantInt::get(ctx.int64Ty, (int64_t )op->func_);
    auto fct_ptr = ctx.getBuilder().CreateIntToPtr(fct_raw, ctx.int64PtrTy);
    auto fct_callee_type = FunctionType::get(ctx.int8PtrTy, {ctx.int8PtrTy}, false);
    auto fct_callee = ctx.getBuilder().CreateBitCast(fct_ptr, fct_callee_type->getPointerTo());

    // get qr_tuple
    auto qrt = ctx.getBuilder().CreateCall(get_qr_tuple, {});

    // execute op func
    auto qr = ctx.getBuilder().CreateCall(fct_callee_type, fct_callee, {qrt});

    if(profiling) {
        t_end = ctx.getBuilder().CreateCall(fadd_now, {});
        ctx.getBuilder().CreateCall(fadd_time_diff, {query_context, ConstantInt::get(ctx.int64Ty, op->op_id_), t_start, t_end});
    }

    // append to result
    ctx.getBuilder().CreateCall(append_to_tuple, {qr});

}

void codegen_inline_visitor::visit(std::shared_ptr<store_op> op) {
    auto obtain_mat_tuple = ctx.extern_func("obtain_mat_tuple");
    auto mat_node = ctx.extern_func("mat_node");
    auto mat_rship = ctx.extern_func("mat_rship");
    auto persist = ctx.extern_func("persist_tuple");
    auto mat_reg = ctx.extern_func("mat_reg_value");

    auto store_entry = BasicBlock::Create(ctx.getContext(), "store_entry", main_function);
    ctx.getBuilder().SetInsertPoint(prev_bb);
    ctx.getBuilder().CreateBr(store_entry);
    ctx.getBuilder().SetInsertPoint(store_entry);

    auto tp = ctx.getBuilder().CreateCall(obtain_mat_tuple, {});
    for(auto & res : reg_query_results) {
        //auto type = ConstantInt::get(ctx.int64Ty, res.type);
        if(res.type == 0) {
            ctx.getBuilder().CreateCall(mat_node, {res.reg_val, tp});
        } else if(res.type == 1) {
            ctx.getBuilder().CreateCall(mat_rship, {res.reg_val, tp});
        } else if(res.type == 2) {
            auto cv_reg = ctx.getBuilder().CreateBitCast(res.reg_val, ctx.int64PtrTy);
            ctx.getBuilder().CreateCall(mat_reg, {gdb, cv_reg, ConstantInt::get(ctx.int64Ty, 92)});
        } else if(res.type == 3) {
            auto cv_reg = ctx.getBuilder().CreateBitCast(res.reg_val, ctx.int64PtrTy);
            ctx.getBuilder().CreateCall(mat_reg, {gdb, cv_reg, ConstantInt::get(ctx.int64Ty, 93)});
        }
    }

    ctx.getBuilder().CreateCall(persist, {gdb, tp});
    prev_bb = store_entry;
}