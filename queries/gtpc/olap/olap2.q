auto matchData = [&](auto res) {
    auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
    auto s = gdb->get_dictionary()->lookup_code(c);
    std::regex r(".*" + %data);
    return std::regex_match(s, r);
});

auto quantityEqual = [&](auto &v) {
    return boost::get<int>(v[1]) == boost::get<int>(v[3]);
});

Limit(100,
    Sort([$2:string ASC, $1:string ASC, $3:uint64 ASC],
        Project([$5.id:uint64, $5.name:string, $7.name:string, $0.id:uint64, $0.name:string, $5.address:string, $5.phone:uint64, $5.comment:string],
            Expand(OUT, "Nation",
                ForeachRelationship(FROM, ":isLocatedIn",
                    Expand(OUT, "Supplier",
                        ForeachRelationship(FROM, ":hasSupplier", $2,
                            Filter(quantityEqual(tuple),
                                Project([$0, $1, $3, $3.quantity:int],
                                    Expand(OUT, "Stock",
                                        ForeachRelationship(FROM, ":hasStock", $0,
                                            GroupBy([$0], 
                                                    [min($1:int)],
                                                Project([$8, $6.quantity:int],
                                                    Filter(matchData($8.data:string),
                                                        Expand(IN, "Item",
                                                            ForeachRelationship(TO, ":hasStock",
                                                                Expand(IN, "Stock",
                                                                    ForeachRelationship(TO, ":hasSupplier",
                                                                        Expand(IN, "Supplier",
                                                                            ForeachRelationship(TO, ":isLocatedIn",
                                                                                Expand(IN, "Nation",
                                                                                    ForeachRelationship(TO, ":isPartOf",
                                                                                        Filter($0.name == %name,
                                                                                            NodeScan("Region")
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
            )
        )
    )
)