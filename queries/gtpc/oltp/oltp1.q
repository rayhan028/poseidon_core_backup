Project([$0.tax:double],
    Filter($0.id == %w_id,
        NodeScan(["Warehouse"])
    )
)

Project([$0.tax:double, $0.next_o_id:int],
    Filter($0.id == %d_id,
        NodeScan(["District"])
    )
)

Update($0 {"next_o_id": %next_o_id},
    Filter($0.id == %d_id,
        NodeScan(["District"])
    )
)

Project([$0.discount:double, $0.last:string, $0.credit:string],
    Filter($0.id == %c_id,
        NodeScan(["Customer"])
    )
)

Create(($0)-[r:hasPlaced {} ]->($1),
    Create(n:Order { "id": %o_id, "entry_d": %entry_d, "carrier_id": %carrier_id, "ol_cnt": %ol_cnt, "all_local": %all_local, "new_order": %new_order},
        Filter($0.id == %c_id,
            NodeScan(["Customer"])
        )
    )
)

for (auto ol_number = 1; ol_number <= %o_ol_cnt; ol_number++) {

    Project([$0.price:double, $0.name:string, $0.data:string],
        Filter($0.id == %i_id,
            NodeScan(["Item"])
        )
    )

    Project([$0.quantity:int, $0.%dist_xx:string, $0.data:string, $0.ytd:int, $0.order_cnt:int, $0.quantity:int, $0.remote_cnt:int],
        Filter($0.id == %s_id,
            NodeScan(["Stock"])
        )
    )

    Update($0 { "ytd": %ytd, "order_cnt": %order_cnt, "quantity": %quantity, "remote_cnt": %remote_cnt},
        Filter($0.id == %s_id,
            NodeScan(["Stock"])
        )
    )

    Create(($1)-[r:hasStock {} ]->($2),
        Crossjoin(
            Create(($0)-[r:contains {} ]->($1),
                Create(n:OrderLine { "id": %ol_id, "number": %number, "delivery_d": %delivery_d, "quantity": %quantity, "amount": %amount, "dist_info": %dist_info},
                    Filter($0.id == %o_id,
                        NodeScan(["Order"])
                    )
                )
            ),
            Filter($0.id == %s_id,
                NodeScan(["Stock"])
            )
        )
    )
}