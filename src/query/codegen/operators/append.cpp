#include "codegen.hpp"


void codegen_inline_visitor::visit(std::shared_ptr<qr_tuple_append> op) {
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

/*
    typedef query_result func_t (const qr_tuple &);
    auto fct_raw = ConstantInt::get(ctx.int64Ty, (int64_t )op->func_.target<func_t>());
    auto fct_ptr = ctx.getBuilder().CreateIntToPtr(fct_raw, ctx.int64PtrTy);
    auto fct_callee_type = FunctionType::get(ctx.int8PtrTy, {ctx.int8PtrTy}, false);
    auto fct_callee = ctx.getBuilder().CreateBitCast(fct_ptr, fct_callee_type->getPointerTo());

    // get qr_tuple
    auto qrt = ctx.getBuilder().CreateCall(get_qr_tuple, {});

    // execute op func
    auto qr = ctx.getBuilder().CreateCall(fct_callee_type, fct_callee, {qrt});
*/
    if(profiling) {
        t_end = ctx.getBuilder().CreateCall(fadd_now, {});
        ctx.getBuilder().CreateCall(fadd_time_diff, {query_context, ConstantInt::get(ctx.int64Ty, op->operator_id_), t_start, t_end});
    }

    // append to result
    ctx.getBuilder().CreateCall(append_to_tuple, {qr});

}