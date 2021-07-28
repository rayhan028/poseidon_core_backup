auto matchState = [&](auto res) {
    auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
    auto s = gdb->get_dictionary()->lookup_code(c);
    std::regex r(%state + ".*");
    return std::regex_match(s, r);
});

Limit(10,
    Sort([$2:double DESC, $1:datetime ASC],
        GroupBy([$0, $2], 
                [sum($1:double)],
            Project([$2.id:uint64, $4.amount:double, $2.entry_d:datetime],
                Filter($4.delivery_d > %date,
                    Expand(OUT, "OrderLine",
                        ForeachRelationship(FROM, ":contains",
                            Filter($2.entry_d < %date,
                                Expand(OUT, "Order",
                                    ForeachRelationship(FROM, ":hasPlaced",
                                        Filter(matchState($0.state:string),
                                            NodeScan("Customer")
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