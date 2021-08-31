if (%y > 60) {

    Project([$0.balance:double, $0.first:string, $0.middle:string, $0.last:string],
        Filter($0.id == %c_id,
            NodeScan(["Customer"])
        )
    )
}
else {

    Limit(1,
        Sort([$1:string ASC],
            Project([$2.balance:double, $2.first:string, $2.middle:string, $2.last:string, $2.id:uint64],
                Filter($2.last:string > %c_last,
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
}

Project([$2.id:uint64, $2.entry_d:datetime, $2.carrier_id:int],
    Expand(OUT, "Order",
        ForeachRelationship(FROM, ":hasPlaced",
            Filter($0.id == %c_id,
                NodeScan(["Customer"])
            )
        )
    )
)

Project([$2.id:uint64, $2.quantity:int, $2.amount:double, $2.delivery_d:datetime],
    Expand(OUT, "OrderLine",
        ForeachRelationship(FROM, ":contains",
            Filter($0.id == %o_id,
                NodeScan(["Order"])
            )
        )
    )
)