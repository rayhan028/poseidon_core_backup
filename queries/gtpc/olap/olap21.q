auto joinPred = [&](auto &lv, auto &rv) {
    return boost::get<node *>(lv[2])->id() == boost::get<node *>(rv[2])->id();
});

auto filterEqual = [&](auto res) {
    return v[6].type() == typeid(null_val);
});

Limit(100,
    Sort([$1:uint64 DESC, $0:string ASC],
        GroupBy([$0], 
                [count($0)],
            Project([$0.name:string, $1],
                Filter(filterEqual(tuple),
                    Outerjoin(joinPred(tuple1, tuple2),
                        Filter($5:datetime > $4:datetime,
                            Project([$2, $4, $6, $8, $8.entry_d:datetime, $6.delivery_d:datetime],
                                Expand(IN, "Order",
                                    ForeachRelationship(TO, ":contains",
                                        Expand(IN, "OrderLine",
                                            ForeachRelationship(TO, ":hasStock",
                                                Expand(IN, "Stock",
                                                    ForeachRelationship(TO, ":hasSupplier",
                                                        Expand(IN, "Supplier",
                                                            ForeachRelationship(TO, ":isLocatedIn",
                                                                Filter($0.name == %name,
                                                                    NodeScan("Nation")
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
                        ),
                        GroupBy([$0, $1, $2, $3],
                            Filter($5:datetime < $8:datetime,
                                Hashjoin([$3, $0],
                                    Filter($5:datetime > $4:datetime,
                                        Project([$2, $4, $6, $8, $8.entry_d:datetime, $6.delivery_d:datetime],
                                            Expand(IN, "Order",
                                                ForeachRelationship(TO, ":contains",
                                                    Expand(IN, "OrderLine",
                                                        ForeachRelationship(TO, ":hasStock",
                                                            Expand(IN, "Stock",
                                                                ForeachRelationship(TO, ":hasSupplier",
                                                                    Expand(IN, "Supplier",
                                                                        ForeachRelationship(TO, ":isLocatedIn",
                                                                            Filter($0.name == %name,
                                                                                NodeScan("Nation")
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
                                    ),
                                    Project([$0, $2, $2.delivery_d:datetime],
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