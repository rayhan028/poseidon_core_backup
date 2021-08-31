Project([$0.next_o_id:int],
    Filter($0.id == %d_id,
        NodeScan(["District"])
    )
)

Count(
    Distinct(
        Project([$10.id:uint64],
            Expand(IN, "Item",
                ForeachRelationship(TO, ":hasStock",
                    Filter($8.quantity:int < %threshold,
                        Expand(OUT, "Stock",
                            ForeachRelationship(FROM, ":hasStock",
                                Expand(OUT, "OrderLine",
                                    ForeachRelationship(FROM, ":contains",
                                        Filter((%d_next_o_id - 200) <= $4.id:uint64 && $4.id:uint64 < %d_next_o_id,
                                            Expand(OUT, "Order",
                                                ForeachRelationship(FROM, ":hasPlaced",
                                                    Expand(OUT, "Customer",
                                                        ForeachRelationship(FROM, ":serves",
                                                            Filter($0.id == %d_id,
                                                                NodeScan(["District"])
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