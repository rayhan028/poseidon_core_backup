auto filterDeliveryD = [&](auto res) {
    auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
    auto edt = %sdt + hours(24 * 365);
    return %sdt <= dt && dt < edt;
});

auto filterEntryD = [&](auto res) {
    return boost::get<ptime>(v[2]) < boost::get<ptime>(v[3]);
});

auto val1 = [&](auto res) {
    auto carrier_id = boost::get<int>(v[1]);
    return (carrier_id == 1 || carrier_id == 2) ?
            query_result(1) : query_result(0);
};

auto val2 = [&](auto res) {
    auto carrier_id = boost::get<int>(v[1]);
    return (carrier_id != 1 && carrier_id != 2) ?
            query_result(1) : query_result(0);
};

Sort([$0:int DESC],
    GroupBy([$0], 
            [sum($4:int), sum($5:int)],
        AppendToTuple(val2(tuple),
            AppendToTuple(val1(tuple),
                Filter(filterEntryD(tuple),
                    Project([$2.ol_cnt:int, $2.carrier_id:int, $2.entry_d:datetime, $0.delivery_d:datetime],
                        Expand(IN, "Order",
                            ForeachRelationship(TO, ":contains",
                                Filter(filterDeliveryD($0),
                                    NodeScan("OrderLine")
                                )
                            )
                        )
                    )
                )
            )
        )
    )
)