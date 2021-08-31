auto filterEntryD = [&](auto res) {
    auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
    auto edt = %sdt + hours(24 * 30 * 3);
    return %sdt <= dt && dt < edt;
});

Limit(20,
    Sort([$2:double DESC],
        GroupBy([$0, $1, $5, $2, $3, $4], 
                [count($1)],
            Project([$2.id:uint64, $2.last:string, $6.amount:double, $2.city:string, $2.phone:uint64, $4.name:string],
                Expand(OUT, "OrderLine",
                    ForeachRelationship(FROM, ":contains", $0,
                        Expand(OUT, "Nation",
                            ForeachRelationship(FROM, ":isLocatedIn",
                                Expand(IN, "Customer",
                                    ForeachRelationship(TO, ":hasPlaced",
                                        Filter(filterEntryD($0.entry_d:datetime),
                                            NodeScan("Order")
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