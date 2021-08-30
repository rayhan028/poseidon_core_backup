auto filterSum = [&](auto res) {
    return boost::get<double>(v[1]) > %amount;
});

Limit(100,
    Sort([$5:double DESC, $3:datetime ASC],
        GroupBy([$0, $1, $2, $3, $4], 
                [sum($5:double)],
            Project([$5.last:string, $5.id:uint64, $0.id:uint64, $0.entry_d:datetime, $0.ol_cnt:int, $3.amount:double],
                Expand(IN, "Customer",
                    ForeachRelationship(TO, ":hasPlaced", $0,
                        Expand(OUT, "OrderLine",
                            ForeachRelationship(FROM, ":contains", $0,
                                Filter(filterSum(tuple),
                                    GroupBy([$0], 
                                            [sum($1:double)],
                                        Project([$0, $2.amount:double],
                                            Expand(OUT, "OrderLine",
                                                ForeachRelationship(FROM, ":contains",
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
    )
)









