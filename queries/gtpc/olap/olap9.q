auto matchData = [&](auto res) {
    auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
    auto s = gdb->get_dictionary()->lookup_code(c);
    std::regex r(".*" + %data);
    return std::regex_match(s, r);
});

Sort([$0:string ASC, $1:int DESC],
    GroupBy([$0, $1], 
            [sum($2:double)],
        Project([$10.name:string, pr_year($6.entry_d:datetime), $4.amount:double],
            Expand(OUT, "Nation",
                ForeachRelationship(FROM, ":isLocatedIn",
                    Expand(OUT, "Supplier",
                        ForeachRelationship(FROM, ":hasSupplier", $2,
                            Expand(IN, "Order",
                                ForeachRelationship(TO, ":contains",
                                    Expand(IN, "OrderLine",
                                        ForeachRelationship(TO, ":hasStock",
                                            Expand(OUT, "Stock",
                                                ForeachRelationship(FROM, ":hasStock",
                                                    Filter(matchData($0.data:string),
                                                        NodeScan("Item")
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
    )
)