Project([$0.name:string, $0.street_1:string, $0.street_2:string, $0.city:string, $0.state:string, $0.zip:string, $0.ytd:double],
    Filter($0.id == %w_id,
        NodeScan(["Warehouse"])
    )
)

Update($0 {"ytd": %ytd},
    Filter($0.id == %w_id,
        NodeScan(["Warehouse"])
    )
)

Project([$0.name:string, $0.street_1:string, $0.street_2:string, $0.city:string, $0.state:string, $0.zip:string, $0.ytd:double],
    Filter($0.id == %d_id,
        NodeScan(["District"])
    )
)

Update($0 {"ytd": %ytd},
    Filter($0.id == %d_id,
        NodeScan(["District"])
    )
)

if (%y > 60) {

    Project([$0.first:string, $0.middle:string, $0.last:string, $0.street_1:string, $0.street_2:string, $0.city:string, $0.state:string,
            $0.zip:string, $0.since:datetime, $0.credit:string, $0.credit_lim:double, $0.discount:double, $0.balance:double, $0.ytd_payment:double,
            $0.payment_cnt:int, $0.data:string],
        Filter($0.id == %d_id,
            NodeScan(["District"])
        )
    )

    Update($0 {"balance": %balance, "ytd_payment": %ytd_payment, "payment_cnt": %payment_cnt},
        Filter($0.id == %c_id,
            NodeScan(["Customer"])
        )
    )
}
else {

    Project([$2.first:string, $2.middle:string, $2.last:string, $2.street_1:string, $2.street_2:string, $2.city:string, $2.state:string,
            $2.zip:string, $2.since:datetime, $2.credit:string, $2.credit_lim:double, $2.discount:double, $2.balance:double, $2.ytd_payment:double,
            $2.payment_cnt:int, $2.data:string],
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

    Update($0 {"balance": %balance, "ytd_payment": %ytd_payment, "payment_cnt": %payment_cnt},
        Filter($0.id == %c_id,
            NodeScan(["Customer"])
        )
    )
}

if (%c_credit == "BC") {

    Update($0 {"data": %data},
        Filter($0.id == %c_id,
            NodeScan(["Customer"])
        )
    )
}