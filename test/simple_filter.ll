; ModuleID = 'S'
source_filename = "simple_filter"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"

%query_ctx = type opaque
%qr_tuple = type opaque
%node = type opaque

define i1 @simple_filter_1(%query_ctx* %0, %qr_tuple* %1) {
init:
    %n = call %node* @qr_get_node(%qr_tuple* %1, i32 0)   
    call void @print_node(%query_ctx* %0, %node* %n)

    %pval = call i64 @get_node_property_int_value(%query_ctx* %0, %node* %n, i64 25)
    %cmp_res = icmp eq i64 %pval, 42
    br i1 %cmp_res, label %finish_true, label %finish_false

finish_false:
    ret i1 false

finish_true:
    ret i1 true
}

declare void @print_node(%query_ctx*, %node*)

declare i64 @get_node_property_int_value(%query_ctx*, %node*, i64)

declare %node* @qr_get_node(%qr_tuple*, i32)