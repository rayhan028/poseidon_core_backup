for (auto d_id = %d_min; d_id <= %d_max; d_id++) {

    Limit(1,
        Sort([$0:uint64 ASC],
            Project([$4.id:uint64, $2.id:uint64, $4.new_order:int, $4.carrier_id:int],
                Filter($4.new_order:int > 0,
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

    Update($0 {"new_order": %new_order, "carrier_id": %carrier_id},
        Filter($0.id == %o_id,
            NodeScan(["Order"])
        )
    )

    Update($0 {"delivery_d": %delivery_d},
        Expand(OUT, "OrderLine",
            ForeachRelationship(FROM, ":contains",
                Filter($0.id == %o_id,
                    NodeScan(["Order"])
                )
            )
        )
    )

    GroupBy([], 
            [sum($0:double)],
        Project([$2.amount:double],
            Expand(OUT, "OrderLine",
                ForeachRelationship(FROM, ":contains",
                    Filter($0.id == %o_id,
                        NodeScan(["Order"])
                    )
                )
            )
        )
    )

    Project([$0.balance:double, $0.delivery_cnt:int],
        Filter($0.id == %c_id,
            NodeScan(["Customer"])
        )
    )

    Update($0 {"balance": %balance, "delivery_cnt": %delivery_cnt},
        Filter($0.id == %c_id,
            NodeScan(["Customer"])
        )
    )
}