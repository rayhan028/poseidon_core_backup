; ModuleID = 'S'
source_filename = "simple_aggregate"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"

%query_ctx = type opaque
%qr_tuple = type opaque
%node = type opaque

%Agg1 = type { i64, i64, i64, i64, double } ; SUM; COUNT; (AVG-SUM, AVG-CNT, AVG)

define void @aggr_1_init(i8* %0, i32 %1) {
    %ptr = bitcast i8* %0 to %Agg1*
    %sum1_ptr = getelementptr inbounds %Agg1, %Agg1* %ptr, i64 0, i32 0
    store i64 0, i64* %sum1_ptr

    %count2_ptr = getelementptr inbounds %Agg1, %Agg1* %ptr, i64 0, i32 1
    store i64 0, i64* %count2_ptr

    %avg_sum3_ptr = getelementptr inbounds %Agg1, %Agg1* %ptr, i64 0, i32 2
    store i64 0, i64* %avg_sum3_ptr

    %avg_count3_ptr = getelementptr inbounds %Agg1, %Agg1* %ptr, i64 0, i32 3
    store i64 0, i64* %avg_count3_ptr

    %avg3_ptr = getelementptr inbounds %Agg1, %Agg1* %ptr, i64 0, i32 4
    store double 0.0, double* %avg3_ptr

    ret void
}

define void @aggr_1_iterate(%query_ctx* %0, i8* %1, i32 %2, %qr_tuple* %3) {
    %ptr = bitcast i8* %1 to %Agg1*

    %n = call %node* @qr_get_node(%qr_tuple* %3, i32 0)   
    call void @print_node(%query_ctx* %0, %node* %n)

    %pval = call i64 @get_node_property_int_value(%query_ctx* %0, %node* %n, i64 25)
    
    %sum1_ptr = getelementptr inbounds %Agg1, %Agg1* %ptr, i64 0, i32 0
    %sum1_val = load i64, i64* %sum1_ptr
    %res1 = add i64 %pval, %sum1_val   ; SUM(id)
    store i64 %res1, i64* %sum1_ptr

    %count2_ptr = getelementptr inbounds %Agg1, %Agg1* %ptr, i64 0, i32 1
    %count2_val = load i64, i64* %count2_ptr
    %res2 = add i64 1, %count2_val   ; COUNT(id)
    store i64 %res2, i64* %count2_ptr

    %avg_sum3_ptr = getelementptr inbounds %Agg1, %Agg1* %ptr, i64 0, i32 2
    %avg_sum3_val = load i64, i64* %avg_sum3_ptr
    %res3 = add i64 %pval, %avg_sum3_val   ; SUM(id)
    store i64 %res3, i64* %avg_sum3_ptr

    %avg_count3_ptr = getelementptr inbounds %Agg1, %Agg1* %ptr, i64 0, i32 3
    %avg_count3_val = load i64, i64* %avg_count3_ptr
    %res4 = add i64 1, %avg_count3_val   ; COUNT(id)
    store i64 %res4, i64* %avg_count3_ptr

    ret void
}

define void @aggr_1_finish(%query_ctx* %0, i8* %1, i32 %2) {
    %ptr = bitcast i8* %1 to %Agg1*
    %avg_sum3_ptr = getelementptr inbounds %Agg1, %Agg1* %ptr, i64 0, i32 2
    %avg_sum3_val = load i64, i64* %avg_sum3_ptr
    %avg_sum3_fval = sitofp i64  %avg_sum3_val to double

    %avg_count3_ptr = getelementptr inbounds %Agg1, %Agg1* %ptr, i64 0, i32 3
    %avg_count3_val = load i64, i64* %avg_count3_ptr
    %avg_count3_fval = sitofp i64  %avg_count3_val to double

    %res = fdiv double %avg_sum3_fval, %avg_count3_fval

    %avg3_ptr = getelementptr inbounds %Agg1, %Agg1* %ptr, i64 0, i32 4

    store double %res, double* %avg3_ptr

    ret void
}

declare void @print_node(%query_ctx*, %node*)

declare i64 @get_node_property_int_value(%query_ctx*, %node*, i64)

declare %node* @qr_get_node(%qr_tuple*, i32)