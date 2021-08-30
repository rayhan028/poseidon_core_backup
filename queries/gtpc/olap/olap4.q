auto filterEntryD = [&](auto res) {
    auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
    auto eqtr = %sqtr + hours(24 * 30 * 3);
    return %sqtr <= dt && dt < eqtr; 
});

auto dateComp = [&](auto &v) {
    return boost::get<ptime>(v[0]) < boost::get<ptime>(v[1]);
});

Sort([$0:int ASC],
    GroupBy([$2], 
            [count($1)],
        Filter(dateComp(tuple),
            Project([$0.entry_d:datetime, $2.delivery_d:datetime, $0.ol_cnt:int],
                Expand(OUT, "OrderLine",
                    ForeachRelationship(FROM, ":contains",
                        Filter(filterEntryD($0.entry_d:datetime),
                            NodeScan("Order")
                        )
                    )
                )
            )
        )
    )
)