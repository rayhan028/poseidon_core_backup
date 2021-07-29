auto matchData = [&](auto res) {
    auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
    auto s = gdb->get_dictionary()->lookup_code(c);
    std::regex r(%data + ".*");
    return std::regex_match(s, r);
});

auto filterDeliveryD = [&](auto res) {
    auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
    auto s = gdb->get_dictionary()->lookup_code(c);
    std::regex r(%data + ".*");
    return std::regex_match(s, r);
});

auto filterQtty = [&](auto res) {
    auto s_qtty = boost::get<int>(v[1]);
    auto ol_qtty = boost::get<int>(v[2]);
    return (2 * s_qtty) > ol_qtty;
});

Sort([$0:string ASC],
    Project([$4.name:string, $4.address:string],
        Filter($6.name:string > %nation,
            Expand(OUT, "Nation",
                ForeachRelationship(FROM, ":isLocatedIn",
                    Expand(OUT, "Supplier",
                        ForeachRelationship(FROM, ":hasSupplier", $0,
                            GroupBy([$0, $1], 
                                    [sum($2:int)],
                                Project([$2, $2.quantity:int, $4.quantity:int],
                                    Filter($4.delivery_d:datetime > %date,
                                        Expand(IN, "OrderLine",
                                            ForeachRelationship(TO, ":hasStock",
                                                Expand(OUT, "Stock",
                                                    ForeachRelationship(FROM, ":hasStock",
                                                        Filter(matchData($0.data:string),
                                                            NodeScan("Item")
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