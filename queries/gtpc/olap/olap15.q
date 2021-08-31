auto filterDeliveryD = [&](auto res) {
    auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
    auto edt = %sdt + hours(24 * 30 * 3);
    return %sdt <= dt && dt < edt;
});

auto filterSum = [&](auto res) {
    return boost::get<double>(v[4]) == boost::get<double>(v[5]);
});

Sort([$0:uint64 ASC],
    Filter(filterSum(tuple),
        Crossjoin(
            Project([$0.id:uint64, $0.name:string, $0.address:string, $0.phone:uint64, $1],
                GroupBy([$0], 
                        [sum($1:double)],
                    AppendToTuple(val1(tuple),
                        Project([$4, $4.amount:double],
                            Expand(OUT, "Supplier",
                                ForeachRelationship(FROM, ":hasSupplier",
                                    Expand(OUT, "Stock",
                                        ForeachRelationship(FROM, ":hasStock",
                                            Filter(filterDeliveryD($0.delivery_d:datetime),
                                                NodeScan("OrderLine")
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                )
            ),
            GroupBy([], 
                    [max($1:double)],
                GroupBy([$0], 
                        [sum($1:double)],
                    AppendToTuple(val1(tuple),
                        Project([$4, $4.amount:double],
                            Expand(OUT, "Supplier",
                                ForeachRelationship(FROM, ":hasSupplier",
                                    Expand(OUT, "Stock",
                                        ForeachRelationship(FROM, ":hasStock",
                                            Filter(filterDeliveryD($0.delivery_d:datetime),
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
        )
    )
)