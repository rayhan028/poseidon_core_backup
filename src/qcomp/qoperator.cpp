#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Utils.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include "qoperator.hpp"

std::string expand_str(EXPAND exp) {
    switch (exp) {
        case EXPAND::IN:
            return "in";
        case EXPAND::OUT:
            return "out";
    }
}

std::string join_op_str(JOIN_OP jop) {
    switch (jop) {
        case JOIN_OP::CROSS:
            return "cross";
        case JOIN_OP::LEFT_OUTER:
            return "left_outer";
    }
}


std::map<int, std::vector<int>> pipeline_types;
std::map<int, GlobalValue*> limits_;
std::map<int, Value*> limit_values_;

int query_id = 0;

void scan_op::codegen(op_visitor & vis, unsigned & op_id, bool interpreted) {
    pipeline_types[query_id].push_back(0);
    op_id_ = op_id;
    auto next_offset = indexed_ ? 3 : 1;

    vis.visit(shared_from_this());

    for(auto & inp : inputs_) {
        inp->codegen(vis, op_id+=next_offset,interpreted);
    }
}



void foreach_rship_op::codegen(op_visitor & vis, unsigned & op_id, bool interpreted) {
    pipeline_types[query_id].push_back(1);
    op_id_ = op_id;

    vis.visit(shared_from_this());

    for(auto & inp : inputs_) {
        inp->codegen(vis, op_id+=1,interpreted);
    }
}



void filter_op::codegen(op_visitor & vis, unsigned & op_id, bool interpreted) {
    op_id_ = op_id;


    vis.visit(shared_from_this());

    for(auto & inp : inputs_) {
        inp->codegen(vis, op_id+=1,interpreted);
    }
}

/*
Value *FilterExpr::codegen(PContext &ctx, Function *consumer) {
    Function *fct = Function::Create(ctx.filterConsumerFctTy, Function::InternalLinkage, "filter_" + op_name_,
                                     ctx.getModule());
    FunctionCallee ls_fct = ctx.extern_func("list_size");

    BasicBlock *bb = BasicBlock::Create(ctx.getContext(), "entry", fct);
    BasicBlock *consume = BasicBlock::Create(ctx.getContext(), "consume", fct);
    BasicBlock *false_pred = BasicBlock::Create(ctx.getContext(), "false", fct);
    ctx.getBuilder().SetInsertPoint(bb);
    auto qrl = fct->args().begin() + 1;

    for (auto &f : fexpr_) {
        f.codegen(fct, qrl, consume, false_pred);
    }

    ctx.getBuilder().SetInsertPoint(consume);
    {
        //ctx.getBuilder().CreateCall(ls_fct, {qrl});
        ctx.getBuilder().CreateRet(nullptr);
    }

    ctx.getBuilder().SetInsertPoint(false_pred);
    {
        //ctx.getBuilder().CreateCall(ls_fct, {qrl});
        ctx.getBuilder().CreateRet(nullptr);
    }

    for (auto &inp : inputs_) {
        inp->codegen(ctx, fct);
    }

    return fct;
}
*/
/*
void FExp::codegen(Function *parent, Value *qr_alloc, BasicBlock *next_true, BasicBlock *next_false) {
    FunctionCallee get_prop_set = ctx.extern_func("pset_get_item_at");

    auto sfwe = FunctionType::get(Type::getVoidTy(ctx.getContext()), {ctx.int64Ty, ctx.int64Ty}, false);
    FunctionCallee stupid_test = ctx.getModule().getOrInsertFunction("stupid_test", sfwe); // TODO: delete debug test

    BasicBlock *qr_type_node = BasicBlock::Create(ctx.getModule().getContext(), "qr_type_node", parent);
    BasicBlock *qr_type_rship = BasicBlock::Create(ctx.getModule().getContext(), "qr_type_rship", parent);
    BasicBlock *pitem_found = BasicBlock::Create(ctx.getModule().getContext(), "pitem_found", parent);
    BasicBlock *foreach_pitem = BasicBlock::Create(ctx.getModule().getContext(), "foreach_pitem", parent);
    BasicBlock *foreach_increment = BasicBlock::Create(ctx.getModule().getContext(), "foreach_pitem_increment", parent);
    BasicBlock *next_prop_it = BasicBlock::Create(ctx.getModule().getContext(), "next_prop_set", parent);
    BasicBlock *while_end = BasicBlock::Create(ctx.getModule().getContext(), "while_end", parent);

    auto UNKNOWN_ID = ConstantInt::get(ctx.int64Ty, std::numeric_limits<int32_t>::max());
    auto MAX_PITEM_CNT = ConstantInt::get(ctx.int64Ty, 3);
    auto cnt_init = ConstantInt::get(ctx.int64Ty, 0);
    auto loop_incr = ConstantInt::get(ctx.int64Ty, 1);

    auto zero_idx = ConstantInt::get(ctx.int64Ty, 0);

    auto rhs_unknown = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    ctx.getBuilder().CreateStore(UNKNOWN_ID, rhs_unknown);

    auto max_pitem = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    ctx.getBuilder().CreateStore(MAX_PITEM_CNT, max_pitem);

    auto loop_cnt = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    ctx.getBuilder().CreateStore(cnt_init, loop_cnt);

    GlobalVariable *propertyStr = ctx.getBuilder().CreateGlobalString(StringRef(property_), property_);
    auto strAlloc = ctx.getBuilder().CreateAlloca(ctx.int8PtrTy);
    auto strPorperty = ctx.getBuilder().CreateBitCast(propertyStr, ctx.int8PtrTy);

    Value *val;
    switch (type_) {
        case FTYPE::INT:
            val = ConstantInt::get(ctx.int64Ty, std::stoi(value_));
            break;
        case FTYPE::DOUBLE:
            val = ConstantFP::get(ctx.doubleTy, std::stod(value_));
            break;
        case FTYPE::STRING:
        case FTYPE::TIME:
        case FTYPE::DATE: {
            GlobalVariable *valueStr = ctx.getBuilder().CreateGlobalString(StringRef(value_), "value");
            auto valStrAlloc = ctx.getBuilder().CreateAlloca(ctx.int8PtrTy);
            auto valStrPtr = ctx.getBuilder().CreateBitCast(valueStr, ctx.int8PtrTy);
            ctx.getBuilder().CreateStore(valStrPtr, valStrAlloc);
            val = ctx.getBuilder().CreateLoad(valStrAlloc);
        }
            break;
    }


    auto gdb_get_dcode = ctx.extern_func("gdb_get_dcode");
    auto gdb = parent->args().begin();
    auto qrl = parent->args().begin() + 1;

    // get back from qr list, extract actual qr and type
    auto back_node = ctx.getBuilder().CreateCall(get_vec_back, {qrl});
    auto qr = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(back_node, 3));
    auto qr_type = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(qr, 1));

    // 1 get dict code
    auto pcode = ctx.getBuilder().CreateCall(gdb_get_dcode, {gdb, strPorperty}); // TODO: 2nd arg ptr?

    // check if node / rship
    //auto node_type = ConstantInt::get(ctx.int64Ty, 0);
    //auto cmp = ctx.getBuilder().CreateICmpEQ(qr_type, node_type);
    //ctx.getBuilder().CreateCondBr(cmp, qr_type_node, qr_type_rship);

    auto res = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(qr, 0));
    auto type = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(qr, 1));
    auto cmp_type = ctx.getBuilder().CreateICmpEQ(type, zero_idx, "cmp_type");
    ctx.getBuilder().CreateCondBr(cmp_type, qr_type_node, qr_type_rship);

    ctx.getBuilder().SetInsertPoint(qr_type_node);
    auto cur_item_arr = ctx.getBuilder().CreateAlloca(ctx.pSetRawArrTy);
    auto cur_item = ctx.getBuilder().CreateAlloca(ctx.pitemTy);
    auto cur_pset = ctx.getBuilder().CreateAlloca(ctx.propertySetPtrTy);


    auto res_node = ctx.getBuilder().CreateBitCast(res, ctx.nodePtrTy);
    res_node->getType()->dump();
    auto plist_id = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(res_node, 4));

    auto plist_id_alloc = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    ctx.getBuilder().CreateStore(plist_id, plist_id_alloc);

    auto loop_body = ctx.while_loop_condition(parent, plist_id_alloc, rhs_unknown, PContext::WHILE_COND::UEQ, while_end,
                                              [&](BasicBlock *body, BasicBlock *epilog) {
                                                  auto pid = ctx.getBuilder().CreateLoad(plist_id_alloc);
                                                  auto pset = ctx.getBuilder().CreateCall(get_prop_set, {gdb, pid});
                                                  ctx.getBuilder().CreateStore(pset, cur_pset);
                                                  auto pitems = ctx.getBuilder().CreateStructGEP(pset, 2);
                                                  auto item_arr = ctx.getBuilder().CreateLoad(
                                                          ctx.getBuilder().CreateStructGEP(pitems, 0));
                                                  ctx.getBuilder().CreateStore(item_arr, cur_item_arr);

                                                  ctx.getBuilder().CreateBr(foreach_pitem);
                                              });

    // foreach item in pitems
    ctx.getBuilder().SetInsertPoint(foreach_pitem);
    auto foreach_pitem_body = ctx.while_loop_condition(parent, loop_cnt, max_pitem, PContext::WHILE_COND::LT,
                                                       next_prop_it, [&](BasicBlock *body, BasicBlock *epilog) {

                auto idx = ctx.getBuilder().CreateLoad(loop_cnt);
                auto item_arr = ctx.getBuilder().CreateLoad(cur_item_arr);
                auto item = ctx.getBuilder().CreateInBoundsGEP(cur_item_arr, {zero_idx, idx});
                ctx.getBuilder().CreateStore(ctx.getBuilder().CreateLoad(item), cur_item);
                auto item_key = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(item, 1));

                auto lhs = ctx.getBuilder().CreateLoad(loop_cnt);
                auto rhs = ctx.getBuilder().CreateLoad(max_pitem);
                item_key->getType()->dump();
                auto key_cmp = ctx.getBuilder().CreateICmp(CmpInst::ICMP_EQ, item_key, pcode, "item.key_eq_pkey");
                ctx.getBuilder().CreateCondBr(key_cmp, pitem_found, foreach_increment);
            });

    ctx.getBuilder().SetInsertPoint(foreach_increment);
    {
        auto idx = ctx.getBuilder().CreateLoad(loop_cnt);
        auto idxpp = ctx.getBuilder().CreateAdd(idx, loop_incr);
        ctx.getBuilder().CreateStore(idxpp, loop_cnt);
        ctx.getBuilder().CreateBr(foreach_pitem_body);
    }

    ctx.getBuilder().SetInsertPoint(next_prop_it);
    {
        // get next prop -> GEP
        auto p_set = ctx.getBuilder().CreateLoad(cur_pset);
        auto next_pset_id = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(p_set, 0));
        ctx.getBuilder().CreateStore(next_pset_id, plist_id_alloc);
        ctx.getBuilder().CreateBr(loop_body);
    }

    ctx.getBuilder().SetInsertPoint(qr_type_rship);
    {
        //TODO: rship handling
        ctx.getBuilder().CreateBr(while_end);
    }

    ctx.getBuilder().SetInsertPoint(pitem_found);
    {
        auto value_arr = ctx.getBuilder().CreateStructGEP(cur_item, 0);

        switch (fop_) {
            case FOP::EQ: {
                switch (type_) {
                    case FTYPE::INT: {
                        // TODO: verify int?
                        auto int_value = ctx.getBuilder().CreateLoad(
                                ctx.getBuilder().CreateBitCast(value_arr, ctx.int64PtrTy));

                        auto cmp_pitem = ctx.getBuilder().CreateICmpEQ(int_value, val);
                        ctx.getBuilder().CreateCondBr(cmp_pitem, next_true, next_false);
                    }
                        break;
                    case FTYPE::DOUBLE:
                    case FTYPE::TIME:
                    case FTYPE::DATE:
                    case FTYPE::STRING:
                        break;
                }
            };
                break;
            case FOP::LT: {

            };
                break;
            case FOP::LE: {

            };
                break;
            case FOP::GT: {

            };
                break;
            case FOP::GE: {

            };
                break;
        }
    }

    ctx.getBuilder().SetInsertPoint(while_end);
    {
        ctx.getBuilder().CreateBr(next_false);
    }

    // 2 search property lists for p_item

     * 1 pset_id = n.prop_list -> GEP node
     * 2 while pset id != UNKNOWN
     * 3 p = properties.at(pset id)
     * 4 foreach item in p.items
     *   if item.key == pcode -> return item
     * 5 pset id = p.next
     * 6 else return empty pitem
     *

    // 3 execute predicate
}*/



/*
void codegen_visitor::visit(std::shared_ptr<project> op) {
    std::map<std::size_t, std::vector<std::pair<std::size_t, dcode_t>>> expr_map;
    std::map<std::size_t, std::vector<FTYPE>> expr_type_map;
    std::map<std::size_t, std::vector<BasicBlock*>> bb_map;
    std::map<std::size_t, BasicBlock*> loop_bb_map;
    std::map<std::size_t, std::vector<BasicBlock*>> or_cmp_map;
    for(auto & e : op->prexpr_) {
        //only for proper fct naming
        op->name_ = op->name_ + e.key;
    }

    int cnt = 0;
    auto func_name = op->name_;
    while(ctx.gen_funcs.find(func_name) != ctx.gen_funcs.end()) {
        func_name = op->name_+std::to_string(++cnt);
    }
    op->name_ = func_name;

    Function *fct = Function::Create(ctx.projectConsumerFctTy, Function::InternalLinkage, func_name,ctx.getModule());
    BasicBlock *bb = BasicBlock::Create(ctx.getContext(), "entry", fct);
    int i = 0;
    for(auto & e : op->prexpr_) {
        auto dc = ctx.get_dcode(e.key);
        expr_map[e.id].push_back({i, dc});
        expr_type_map[e.id].push_back(e.type);
        i++;
        bb_map[e.id].push_back(BasicBlock::Create(ctx.getContext(), "bb_"+e.key, fct));
    }


    auto max_ =
            std::max_element(op->prexpr_.begin(), op->prexpr_.end(),
                             [](pr_expr &e1, pr_expr &e2) { return e1.id < e2.id; });

    auto apply_pexpr = ctx.extern_func("apply_pexpr");
    auto add_end = ctx.gen_funcs["qr_list_add_end"];

    auto sfwe = FunctionType::get(Type::getVoidTy(ctx.getContext()), {ctx.int64Ty, ctx.int8Ty}, false);
    FunctionCallee stupid_test = ctx.getModule().getOrInsertFunction("stupid_test", sfwe); //TODO: delete debug test

    BasicBlock *consume = BasicBlock::Create(ctx.getContext(), "consume", fct);

    auto qr_at = ctx.qr_list_at();
    auto gdb = fct->args().begin();
    auto tid = fct->args().begin() + 1;
    auto qrl = fct->args().begin() + 2;

    ctx.getBuilder().SetInsertPoint(bb);
    auto max_cnt_alloc = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    ctx.getBuilder().CreateStore(ctx.MAX_PITEM_CNT, max_cnt_alloc);
    auto presultTy = ArrayType::get(ctx.qrResultPtrTy, op->prexpr_.size());
    auto presultAlloc = ctx.getBuilder().CreateAlloca(presultTy);

    auto nqrl = ctx.create_qr_list();


    auto next_it = op->prexpr_.begin();
    ctx.getBuilder().CreateBr(*bb_map[next_it->id].begin());
    std::set<std::size_t> it_set;
    std::set<std::size_t> it_or_set;
    for(auto & p : op->prexpr_) {
        if(it_set.find(p.id) != it_set.end())
            continue;
        it_set.insert(p.id);

        auto sbb = bb_map[p.id].begin();
        next_it++;
        int ibb = 0;
        for(auto & b : bb_map[p.id]) {
            ctx.getBuilder().SetInsertPoint(b);

            sbb++;

            BasicBlock *nextbb;
            while(next_it != op->prexpr_.end()) {
                if(it_set.find(next_it->id) != it_set.end())
                    next_it++;
                break;
            }
            if (sbb == bb_map[p.id].end())
                if(next_it == op->prexpr_.end())
                    nextbb = consume;
                else
                    nextbb = *bb_map[next_it->id].begin();
            else
                nextbb = *sbb;

            BasicBlock *pitem_found = BasicBlock::Create(ctx.getContext(), "pitem_found", fct);
            BasicBlock *add_int_res = BasicBlock::Create(ctx.getContext(), "add_int_res", fct);
            BasicBlock *add_str_res = BasicBlock::Create(ctx.getContext(), "add_str_res", fct);
            BasicBlock *check_str_ty = BasicBlock::Create(ctx.getContext(), "check_str_ty", fct);

            for (auto &em : expr_map) {
                for (auto &o : em.second) {
                    or_cmp_map[p.id].push_back(
                            BasicBlock::Create(ctx.getContext(), "key_cmp_or" + std::to_string(o.first), fct));
                }
            }

            auto for_each_increment = BasicBlock::Create(ctx.getContext(), "for_each_increment_" + std::to_string(p.id),
                                                         fct);
            auto next_prop_it = BasicBlock::Create(ctx.getContext(), "next_prop_" + std::to_string(p.id), fct);

            auto rhs_unknown = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
            ctx.getBuilder().CreateStore(ctx.UNKNOWN_ID, rhs_unknown);
            auto qn = ctx.getBuilder().CreateCall(qr_at, {qrl, ConstantInt::get(ctx.int64Ty, p.id)});
            auto qar = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(qn, 3));
            qar->getType()->dump();
            auto qr = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(qar, 0));
            auto res_node = ctx.getBuilder().CreateBitCast(qr, ctx.nodePtrTy);
            auto plist_id = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(res_node, 4));
            auto plist_id_alloc = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
            auto cur_item = ctx.getBuilder().CreateAlloca(ctx.pitemTy);
            auto cur_pset = ctx.getBuilder().CreateAlloca(ctx.propertySetPtrTy);
            auto cur_item_arr = ctx.getBuilder().CreateAlloca(ctx.pSetRawArrTy);
            auto cur_dcode = ctx.getBuilder().CreateAlloca(ctx.int32Ty);

            auto type_alloc = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
            auto pos_alloc = ctx.getBuilder().CreateAlloca(ctx.int64Ty);

            ctx.getBuilder().CreateStore(plist_id, plist_id_alloc);

            auto loop_cnt = ctx.getBuilder().CreateAlloca(ctx.int64Ty);

            loop_bb_map[p.id] = BasicBlock::Create(ctx.getContext(), "foreach_item_for_" + std::to_string(p.id), fct);
            auto loop_body = ctx.while_loop_condition(fct, plist_id_alloc, rhs_unknown, ctx.WHILE_COND::LT, nextbb,
                                                      [&](BasicBlock *body, BasicBlock *epilog) {
                                                          auto pid = ctx.getBuilder().CreateLoad(plist_id_alloc);
                                                          auto pset = ctx.getBuilder().CreateCall(
                                                                  ctx.extern_func("pset_get_item_at"),
                                                                  {gdb, pid});


                                                          ctx.getBuilder().CreateStore(pset, cur_pset);
                                                          auto pitems = ctx.getBuilder().CreateStructGEP(pset, 2);
                                                          auto item_arr = ctx.getBuilder().CreateLoad(
                                                                  ctx.getBuilder().CreateStructGEP(pitems, 0));
                                                          ctx.getBuilder().CreateStore(item_arr, cur_item_arr);
                                                          ctx.getBuilder().CreateStore(ctx.LLVM_ZERO, loop_cnt);
                                                          ctx.getBuilder().CreateBr(loop_bb_map[p.id]);
                                                      });

            ctx.getBuilder().SetInsertPoint(loop_bb_map[p.id]);

            auto foreach_pitem_body = ctx.while_loop_condition(fct, loop_cnt, max_cnt_alloc, ctx.WHILE_COND::LT,
                                                               next_prop_it, [&](BasicBlock *body, BasicBlock *epilog) {
                        auto idx = ctx.getBuilder().CreateLoad(loop_cnt);
                        auto item = ctx.getBuilder().CreateInBoundsGEP(cur_item_arr, {ctx.LLVM_ZERO, idx});
                        ctx.getBuilder().CreateStore(ctx.getBuilder().CreateLoad(item), cur_item);
                        auto item_key = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(item, 1));

                        ctx.getBuilder().CreateBr(or_cmp_map[p.id].at(ibb));
                        auto ty_ct = 0;
                        for (auto &ex : expr_map[p.id]) {
                            ctx.getBuilder().SetInsertPoint(or_cmp_map[p.id].at(ibb));
                            ty_ct++;
                            ibb++;
                            auto pcode = ConstantInt::get(ctx.int32Ty, APInt(32, ex.second));
                            ctx.getBuilder().CreateStore(pcode, cur_dcode);
                            auto pos = ConstantInt::get(ctx.int64Ty, ex.first);
                            auto etype = ConstantInt::get(ctx.int64Ty,
                                                          static_cast<int>(expr_type_map[p.id].at(ty_ct - 1)));
                            ctx.getBuilder().CreateStore(pos, pos_alloc);
                            ctx.getBuilder().CreateStore(etype, type_alloc);
                            auto key_cmp = ctx.getBuilder().CreateICmp(CmpInst::ICMP_EQ, item_key, pcode,
                                                                       "item.key_eq_pkey");
                            auto bb_it = or_cmp_map[p.id].at(ibb)->begin();
                            bool last_set = false;
                            while(bb_it != or_cmp_map[p.id].at(ibb)->end()) {
                                bb_it++;
                                last_set = true;
                                if (bb_it == or_cmp_map[p.id].at(ibb)->end()) {
                                    ctx.getBuilder().CreateCondBr(key_cmp, pitem_found, for_each_increment);
                                }
                                else
                                    ctx.getBuilder().CreateCondBr(key_cmp, pitem_found, (BasicBlock*)&bb_it);
                            }
                            if(!last_set)
                                ctx.getBuilder().CreateCondBr(key_cmp, pitem_found, for_each_increment);
                        }
                    });


            ctx.getBuilder().SetInsertPoint(for_each_increment);
            {
                auto idx = ctx.getBuilder().CreateLoad(loop_cnt);
                auto idxpp = ctx.getBuilder().CreateAdd(idx, ctx.LLVM_ONE);
                ctx.getBuilder().CreateStore(idxpp, loop_cnt);
                ctx.getBuilder().CreateBr(foreach_pitem_body);
            }

            ctx.getBuilder().SetInsertPoint(pitem_found);
            {
                auto cmp_itype = ctx.getBuilder().CreateICmpEQ(ctx.getBuilder().CreateLoad(type_alloc), ctx.LLVM_ZERO);
                ctx.getBuilder().CreateCondBr(cmp_itype, add_int_res, check_str_ty);
            }

            ctx.getBuilder().SetInsertPoint(check_str_ty);
            {
                auto cmp_itype = ctx.getBuilder().CreateICmpEQ(ctx.getBuilder().CreateLoad(type_alloc),
                                                               ConstantInt::get(ctx.int64Ty, 2));
                ctx.getBuilder().CreateCondBr(cmp_itype, add_str_res, consume);
            }

            ctx.getBuilder().SetInsertPoint(add_int_res);
            {
                auto value_arr = ctx.getBuilder().CreateStructGEP(cur_item, 0);

                auto transformed_result = ctx.getBuilder().CreateAlloca(ctx.qrResultTy);
                auto ival = ctx.getBuilder().CreateBitCast(value_arr, ctx.int8PtrTy);

                auto lhs2 = ctx.getBuilder().CreateLoad(plist_id_alloc);
                auto rhs2 = ctx.getBuilder().CreateLoad(ival);
                ctx.getBuilder().CreateCall(stupid_test, {lhs2, rhs2});

                auto ival_field = ctx.getBuilder().CreateStructGEP(transformed_result, 0);
                ctx.getBuilder().CreateStore(ival, ival_field);
                auto res_type_field = ctx.getBuilder().CreateStructGEP(transformed_result, 1);
                auto res_type = ConstantInt::get(ctx.int64Ty, 2);
                ctx.getBuilder().CreateStore(res_type, res_type_field);
                auto res_null_field = ctx.getBuilder().CreateStructGEP(transformed_result, 2);
                auto res_null = ConstantInt::get(ctx.int8Ty, 0);
                ctx.getBuilder().CreateStore(res_null, res_null_field);

                auto pos = ctx.getBuilder().CreateLoad(pos_alloc);
                auto arr = ctx.getBuilder().CreateInBoundsGEP(presultAlloc, {ctx.LLVM_ZERO, pos});
                auto tr = ctx.getBuilder().CreateLoad(transformed_result);
                tr->getType()->dump();
                arr->getType()->dump();
                ctx.getBuilder().CreateStore(transformed_result, arr);
                ctx.getBuilder().CreateBr(next_prop_it);
            }

            ctx.getBuilder().SetInsertPoint(add_str_res);
            {
                auto value_arr = ctx.getBuilder().CreateStructGEP(cur_item, 0);
                auto lookup_dc = ctx.extern_func("dict_lookup_dcode");

                auto transformed_result = ctx.getBuilder().CreateAlloca(ctx.qrResultTy);
                auto ival = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateBitCast(value_arr, ctx.int32PtrTy));
                auto sval = ctx.getBuilder().CreateCall(lookup_dc, {gdb, ival});

                auto lhs2 = ctx.getBuilder().CreateLoad(plist_id_alloc);
                auto rhs2 = ctx.getBuilder().CreateLoad(sval);
                ctx.getBuilder().CreateCall(stupid_test, {lhs2, rhs2});

                auto ival_field = ctx.getBuilder().CreateStructGEP(transformed_result, 0);
                ctx.getBuilder().CreateStore(sval, ival_field);
                auto res_type_field = ctx.getBuilder().CreateStructGEP(transformed_result, 1);
                auto res_type = ConstantInt::get(ctx.int64Ty, 4);
                ctx.getBuilder().CreateStore(res_type, res_type_field);
                auto res_null_field = ctx.getBuilder().CreateStructGEP(transformed_result, 2);
                auto res_null = ConstantInt::get(ctx.int8Ty, 0);
                ctx.getBuilder().CreateStore(res_null, res_null_field);

                auto pos = ctx.getBuilder().CreateLoad(pos_alloc);
                auto arr = ctx.getBuilder().CreateInBoundsGEP(presultAlloc, {ctx.LLVM_ZERO, pos});
                auto tr = ctx.getBuilder().CreateLoad(transformed_result);
                tr->getType()->dump();
                arr->getType()->dump();
                ctx.getBuilder().CreateStore(transformed_result, arr);
                ctx.getBuilder().CreateBr(next_prop_it);
            }

            ctx.getBuilder().SetInsertPoint(next_prop_it);
            {
// get next prop -> GEP
                auto p_set = ctx.getBuilder().CreateLoad(cur_pset);
                auto next_pset_id = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(p_set, 0));
                ctx.getBuilder().CreateStore(next_pset_id, plist_id_alloc);
                ctx.getBuilder().CreateBr(loop_body);
            }
        }

    }

    ctx.getBuilder().SetInsertPoint(consume);
    {
        for(int j = 0; j < op->prexpr_.size(); j++) {
            auto idx = ConstantInt::get(ctx.int64Ty, j);
            auto arr = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(presultAlloc, {ctx.LLVM_ZERO, idx}));
            ctx.getBuilder().CreateCall(add_end, {nqrl, arr});
        }
        ctx.getBuilder().CreateCall(op->consumer_, {gdb, tid, nqrl});
        ctx.getBuilder().CreateRet(nullptr);
    }

    ctx.gen_funcs[op->name_] = fct;
    for (auto &inp : op->inputs_) {
        inp->set_consumer(fct);
    }
}
*/

void project::codegen(op_visitor & vis, unsigned & op_id, bool interpreted) {
    op_id_ = op_id;

    vis.visit(shared_from_this());

    for(auto & inp : inputs_) {
        inp->codegen(vis, op_id,interpreted);
    }
}



void expand_op::codegen(op_visitor & vis, unsigned & op_id, bool interpreted) {
    pipeline_types[query_id].push_back(0);
    op_id_ = op_id;


    vis.visit(shared_from_this());

    for(auto & inp : inputs_) {
        inp->codegen(vis, op_id+=1,interpreted);
    }
}

int get_nopid(int & start, std::vector<algebra_optr> & ops, join_op endop) {
    auto cur = ops.back();
    ops.pop_back();
    while(cur->op_id_ != endop.op_id_) {
        start++;
        if(cur->type_ == qop_type::left_join || cur->type_ == qop_type::cross_join) {
            ops.push_back(cur->inputs_[0]);
            cur = cur->inputs_[1];
        } else {
            if(cur->type_ == qop_type::none) {
                if(ops.empty()) {
                    break;
                } else {
                    cur = ops.back();
                    ops.pop_back();
                }
            } else {
                cur = cur->inputs_[0];
            }
        }

    }
    return start;
}

void join_op::codegen(op_visitor & vis, unsigned & op_id, bool interpreted) {
    op_id_ = op_id;

    auto lhs_op_id = op_id_+1;
    auto cur_op = inputs_[1];

    if(!interpreted) {
        inputs_[1]->codegen(vis, op_id, false);
    }

    vis.visit(shared_from_this());

    if(interpreted) {
        for(auto & inp : inputs_) {
            inp->codegen(vis, op_id,true);
        }
    } else {
        inputs_[0]->codegen(vis, op_id, false);
    }

}

Function *join_op::codegen_rhs(PContext &ctx, Function *consumer) {
    int cnt = 0;
    auto func_name = name_;
    while(ctx.gen_funcs.find(func_name) != ctx.gen_funcs.end()) {
        func_name = name_+std::to_string(++cnt);
    }

    func_name = func_name + join_op_str(jop_) + "lhs";

    name_ = func_name;

    FunctionCallee rship_by_id = ctx.extern_func("rship_by_id");

    Function *fct = Function::Create(ctx.consumerFctTy, Function::ExternalLinkage,
                                     name_, ctx.getModule());
    BasicBlock *entry = BasicBlock::Create(ctx.getContext(), "entry", fct);
    BasicBlock *concat_qrl = BasicBlock::Create(ctx.getContext(), "concat_qrl", fct);
    BasicBlock *incr_loop = BasicBlock::Create(ctx.getContext(), "incr_loop", fct);
    BasicBlock *consume = BasicBlock::Create(ctx.getContext(), "consume", fct);
    BasicBlock *foreach_rship = BasicBlock::Create(ctx.getContext(), "foreach_rship", fct);
    BasicBlock *next_rship = BasicBlock::Create(ctx.getContext(), "next_rship", fct);
    BasicBlock *consume_next = BasicBlock::Create(ctx.getContext(), "consume_next_left", fct);
    BasicBlock *for_each_rship = BasicBlock::Create(ctx.getContext(), "for_each_rship", fct);
    BasicBlock *for_each_next = BasicBlock::Create(ctx.getContext(), "for_each_next_rship", fct);
    BasicBlock *end = BasicBlock::Create(ctx.getContext(), "end", fct);

    Value *left_pos;
    Value *right_pos;

    auto get_join_vec = ctx.extern_func("get_join_vec_arr");
    auto get_join_vec_size = ctx.extern_func("get_join_vec_size");

    ctx.getBuilder().SetInsertPoint(entry);
    if(jop_ == JOIN_OP::LEFT_OUTER) {
        left_pos = ConstantInt::get(ctx.int64Ty, join_pos_.first);
        right_pos = ConstantInt::get(ctx.int64Ty, join_pos_.second);
    }

    auto gdb = fct->args().begin();
    auto oid = fct->args().begin() + 1;
    auto qr_tuple_list = fct->args().begin() + 2;
    auto rs = fct->args().begin() + 3;
    auto prev_size = fct->args().begin() + 4;
    auto ty = fct->args().begin() + 5;
    auto call_map_arg = fct->args().begin() + 6;
    auto offset = fct->args().begin() + 7;
    auto call_map = ctx.getBuilder().CreateBitCast(fct->args().begin() + 6, ctx.callMapPtrTy);

    auto lhs_qr_arr = ctx.getBuilder().CreateBitCast(qr_tuple_list, ctx.res_arr_type->getPointerTo());
    auto lhs_size_field = ctx.getBuilder().CreateInBoundsGEP(lhs_qr_arr, {ctx.LLVM_ZERO, ctx.LLVM_ZERO});
    auto lhs_size = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateLoad(lhs_size_field));
    auto insert_pos = ctx.getBuilder().CreateAdd(prev_size, ctx.LLVM_ONE);

    auto lhs_alloca = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    auto rhs_alloca = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    auto rhs_id_alloca = ctx.getBuilder().CreateAlloca(ctx.int64Ty);

    auto max_idx = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    auto cur_idx = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    ctx.getBuilder().CreateStore(ctx.LLVM_ZERO, cur_idx);

    //get input vector from join obj
    auto inputs_vec_raw = ConstantInt::get(ctx.int64Ty, (int64_t )&join_inputs_);
    auto inputs_vec = ctx.getBuilder().CreateIntToPtr(inputs_vec_raw, ctx.int64PtrTy);

    auto qrl_size = ctx.getBuilder().CreateCall(get_join_vec_size, {inputs_vec});
    ctx.getBuilder().CreateStore(qrl_size, max_idx);

    auto cur_qr = ctx.getBuilder().CreateAlloca(ctx.int64PtrTy);

    auto cur_lhs_pos = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    auto cur_rhs_pos = ctx.getBuilder().CreateAlloca(ctx.int64Ty);

    auto cpy_size = ctx.getBuilder().CreateAlloca(ctx.int64Ty);

    auto noid = ctx.getBuilder().CreateAdd(oid, ctx.LLVM_ONE);
    auto fct_ptr = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(call_map, {ctx.LLVM_ZERO, oid}));

    //auto ql = ctx.getBuilder().CreateCall(get_join_vec, {inputs_vec, ctx.LLVM_TWO});
    auto loop_body = ctx.while_loop_condition(fct, cur_idx, max_idx, PContext::WHILE_COND::LT, end,
                                              [&](BasicBlock *body, BasicBlock *epilog) {
        // get current index for join vector
        auto idx = ctx.getBuilder().CreateLoad(cur_idx);

        // get tuple at index from vector
        //auto qrlp = ctx.getBuilder().CreateInBoundsGEP(ql, {ctx.LLVM_ZERO});
        //auto qrl = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(qrlp, {idx}));
        auto qrl = ctx.getBuilder().CreateCall(get_join_vec, {inputs_vec, idx});

        //qrl->getType()->dump();
        // store current tuple to stack
        ctx.getBuilder().CreateStore(qrl, cur_qr);

        // process join
        if(jop_ == JOIN_OP::CROSS)
            ctx.getBuilder().CreateBr(concat_qrl);
        else
            ctx.getBuilder().CreateBr(foreach_rship);
                                              });

    ctx.getBuilder().SetInsertPoint(foreach_rship);
    auto rhs_qr_ptr_lj = ctx.getBuilder().CreateLoad(cur_qr);
    auto rhs_qr_arr_lj = ctx.getBuilder().CreateBitCast(rhs_qr_ptr_lj, ctx.res_arr_type->getPointerTo());
    auto rhs_node_lj = ctx.getBuilder().CreateBitCast(ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(rhs_qr_arr_lj, {ctx.LLVM_ZERO, right_pos})), ctx.nodePtrTy);
    auto lhs_node_lj = ctx.getBuilder().CreateBitCast(ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(qr_tuple_list, {ctx.LLVM_ZERO, left_pos})), ctx.nodePtrTy);

    auto rhs_node_id = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(rhs_node_lj, 1));


    auto lhs_node_rship_id = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(lhs_node_lj, 2));
    ctx.getBuilder().CreateStore(lhs_node_rship_id, lhs_alloca);

    ctx.getBuilder().CreateStore(ctx.UNKNOWN_ID, rhs_alloca);

    Value *rship;

    auto loop_body_lf = ctx.while_loop_condition(fct, lhs_alloca, rhs_alloca, PContext::WHILE_COND::LT, end,
                                              [&](BasicBlock *, BasicBlock *) {
                                                  auto lhs = ctx.getBuilder().CreateLoad(lhs_alloca);

                                                  rship = ctx.getBuilder().CreateCall(rship_by_id, {gdb, lhs});
                                                  auto to_nod_id = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(rship, 3));

                                                  auto id_eq = ctx.getBuilder().CreateICmpEQ(to_nod_id, rhs_node_id);
                                                  ctx.getBuilder().CreateCondBr(id_eq, concat_qrl, next_rship);
                                              });

    ctx.getBuilder().SetInsertPoint(next_rship);
    auto *nrship = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateStructGEP(rship, 4));
    ctx.getBuilder().CreateStore(nrship, lhs_alloca);
    ctx.getBuilder().CreateBr(loop_body_lf);

    ctx.getBuilder().SetInsertPoint(concat_qrl);
    // increment index and store
    auto idx = ctx.getBuilder().CreateLoad(cur_idx);
    auto nidx = ctx.getBuilder().CreateAdd(idx, ctx.LLVM_ONE);
    ctx.getBuilder().CreateStore(nidx, cur_idx);

    auto rhs_qr_ptr = ctx.getBuilder().CreateLoad(cur_qr);
    auto rhs_qr_arr = ctx.getBuilder().CreateBitCast(rhs_qr_ptr, ctx.res_arr_type->getPointerTo());
    auto rhs_size_field = ctx.getBuilder().CreateInBoundsGEP(rhs_qr_arr, {ctx.LLVM_ZERO, ctx.LLVM_ZERO});

    // get size of tuples from rhs
    auto rhs_size_ptr = ctx.getBuilder().CreateLoad(rhs_size_field);
    auto rhs_size = ctx.getBuilder().CreateLoad(rhs_size_ptr);

    // get insert position of rhs -> size + 1
    auto rhs_pos_max = ctx.getBuilder().CreateAlloca(ctx.int64Ty);
    ctx.getBuilder().CreateStore(rhs_size, rhs_pos_max);

    // store intitial lhs & rhs index to stack
    ctx.getBuilder().CreateStore(ctx.LLVM_ONE, cur_lhs_pos);

    ctx.getBuilder().CreateStore(ctx.LLVM_ONE, cur_rhs_pos);

    ctx.getBuilder().CreateStore(prev_size, cpy_size);

    auto offset_rhs = ctx.getBuilder().CreateAdd(rhs_size, ctx.LLVM_ONE);
    auto nsize = ctx.getBuilder().CreateAdd(prev_size, offset_rhs);

    // copy each element from rhs to lhs
    auto copy_body = ctx.while_loop_condition(fct, cur_lhs_pos, rhs_pos_max, PContext::WHILE_COND::LE, consume,
                                              [&](BasicBlock *body, BasicBlock *epilog) {
      auto lhs_pos = ctx.getBuilder().CreateLoad(cur_lhs_pos);
      auto lhs_dst = ctx.getBuilder().CreateInBoundsGEP(lhs_qr_arr, {ctx.LLVM_ZERO, lhs_pos});

      auto rhs_pos = ctx.getBuilder().CreateLoad(cur_rhs_pos);
      auto rhs_src = ctx.getBuilder().CreateLoad(ctx.getBuilder().CreateInBoundsGEP(rhs_qr_arr, {ctx.LLVM_ZERO, rhs_pos}));

      ctx.getBuilder().CreateStore(rhs_src, lhs_dst);
      ctx.getBuilder().CreateBr(incr_loop);
    });

    ctx.getBuilder().SetInsertPoint(incr_loop);
    auto lhs_pos = ctx.getBuilder().CreateLoad(cur_lhs_pos);
    auto incr_lhs = ctx.getBuilder().CreateAdd(lhs_pos, ctx.LLVM_ONE);
    ctx.getBuilder().CreateStore(incr_lhs, cur_lhs_pos);
    auto rhs_pos = ctx.getBuilder().CreateLoad(cur_rhs_pos);
    auto incr_rhs = ctx.getBuilder().CreateAdd(rhs_pos, ctx.LLVM_ONE);
    ctx.getBuilder().CreateStore(incr_rhs, cur_rhs_pos);
    ctx.getBuilder().CreateBr(copy_body);

    ctx.getBuilder().SetInsertPoint(consume);
    auto forward = ctx.getBuilder().CreateBitCast(lhs_qr_arr, ctx.int64PtrTy);
    ctx.getBuilder().CreateCall(fct_ptr, {gdb, noid, forward, rs, nsize, ty, call_map_arg, offset});
    ctx.getBuilder().CreateBr(loop_body);


    ctx.getBuilder().SetInsertPoint(end);
    {
        ctx.getBuilder().CreateRetVoid();
    }


    ctx.gen_funcs[name_] = fct;
    return fct;
}



void collect_op::codegen(op_visitor & vis, unsigned & op_id, bool interpreted) {
    op_id_ = op_id;

    vis.visit(shared_from_this());
}



void sort_op::codegen(op_visitor & vis, unsigned & op_id, bool interpreted) {
    op_id_ = op_id;

    vis.visit(shared_from_this());

    for(auto & inp : inputs_) {
        inp->codegen(vis, op_id+=1,true);
    }
}

std::function<bool(const qr_tuple &, const qr_tuple &)> sort_op::cmp_ = 0;



void limit_op::codegen(op_visitor &vis, unsigned int & op_id, bool interpreted) {
    op_id_ = op_id;
    vis.visit(shared_from_this());

    for(auto & inp : inputs_) {
        inp->codegen(vis, op_id+=1,true);
    }
}



void end_op::codegen(op_visitor &vis, unsigned int & op_id, bool interpreted) {
    vis.visit(shared_from_this());
}

void create_op::codegen(op_visitor &vis, unsigned int & op_id, bool interpreted) {
    op_id_ = op_id;

    vis.visit(shared_from_this());

    for(auto & inp : inputs_) {
        inp->codegen(vis, op_id+=2,true);
    }
}

