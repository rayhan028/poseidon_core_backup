auto joinPred = [&](auto &lv, auto &rv) {
    auto connected = false;
    auto src = boost::get<node *>(lv[0]);
    auto des = boost::get<node *>(rv[0]);
    gdb->foreach_from_relationship_of_node((*src), [&](auto &r) {
        if (r.to_node_id() == des->id())
        connected = true;
    });
    return connected;
});

auto filterMatches = [&](auto res) {
    return v[1].type() == typeid(null_val);
});

auto append0 = [&](auto res) {
    auto carrier_id = boost::get<int>(v[1]);
    return (carrier_id == 1 || carrier_id == 2) ?
            query_result(1) : query_result(0);
};


Sort([$1:uint64 DESC, $0:uint64 DESC],
    GroupBy([$1], 
            [count($0)],
        Union(
            GroupBy([$2], 
                    [count($0)],
                Expand(IN, "Customer",
                    ForeachRelationship(TO, ":hasPlaced",
                        Filter($0.carrier_id > %carrier_id,
                            NodeScan("Order")
                        )
                    )
                )
            ),
            Project([$0, $2],
                AppendToTuple(append0(tuple),
                    Filter(filterMatches($0),
                        Outerjoin(joinPred(tuple1, tuple2),
                            Filter($0.carrier_id > %carrier_id,
                                NodeScan("Order")
                            ),
                            NodeScan("Customer")
                        )
                    )
                )
            )
        )
    )
)

                                                    