Sort([$0:string ASC, $1:string ASC, $2:int ASC],
    GroupBy([$0, $1, $2], 
            [sum($3:double)],
        Union(
            Project([$12.name:string, $0.name:string, pr_year($6.delivery_d:datetime), $6.amount:datetime],
                Filter($12.name == %name1,
                    Expand(OUT, "Nation",
                        ForeachRelationship(FROM, ":isLocatedIn",
                            Expand(OUT, "Supplier",
                                ForeachRelationship(FROM, ":hasSupplier",
                                    Expand(OUT, "Stock",
                                        ForeachRelationship(FROM, ":hasStock",
                                            Filter(%sdt <= $6.delivery_d:datetime && $6.delivery_d:datetime < %edt,
                                                Expand(OUT, "OrderLine",
                                                    ForeachRelationship(FROM, ":contains",
                                                        Expand(OUT, "Order",
                                                            ForeachRelationship(FROM, ":hasPlaced",
                                                                Expand(IN, "Customer",
                                                                    ForeachRelationship(TO, ":isLocatedIn",
                                                                        Filter($0.name == %name2,
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
                                )
                            )
                        )
                    )
                )
            ),
            Project([$12.name:string, $0.name:string, pr_year($6.delivery_d:datetime), $6.amount:datetime],
                Filter($12.name == %name2,
                    Expand(OUT, "Nation",
                        ForeachRelationship(FROM, ":isLocatedIn",
                            Expand(OUT, "Supplier",
                                ForeachRelationship(FROM, ":hasSupplier",
                                    Expand(OUT, "Stock",
                                        ForeachRelationship(FROM, ":hasStock",
                                            Filter(%sdt <= $6.delivery_d:datetime && $6.delivery_d:datetime < %edt,
                                                Expand(OUT, "OrderLine",
                                                    ForeachRelationship(FROM, ":contains",
                                                        Expand(OUT, "Order",
                                                            ForeachRelationship(FROM, ":hasPlaced",
                                                                Expand(IN, "Customer",
                                                                    ForeachRelationship(TO, ":isLocatedIn",
                                                                        Filter($0.name == %name1,
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
                                )
                            )
                        )
                    )
                )
            )
        )
    )
)