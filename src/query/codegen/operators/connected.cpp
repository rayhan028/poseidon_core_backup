#include "codegen.hpp"

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