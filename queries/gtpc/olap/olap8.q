auto matchData = [&](auto res) {
    auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
    auto s = gdb->get_dictionary()->lookup_code(c);
    std::regex r(".*" + data);
    return std::regex_match(s, r);
});

auto val1 = [&](auto &v) {
    bool b = boost::get<std::string>(v[1]) == nation;
    return b ? boost::get<double>(v[2]) : 0.0;
};

auto val2 = [&](auto &v) {
    return boost::get<double>(v[2]) / (double)boost::get<double>(v[1]);
};

Project([$0, $3],
    AppendToTuple(val2(tuple),
        GroupBy([$0], 
                [sum($2:double), sum($3:double)],
            AppendToTuple(val1(tuple),
                Project([pr_year($6.entry_d:datetime), $16.name:string, $8.amount:double],
                    Expand(OUT, "Nation",
                        ForeachRelationship(FROM, ":isLocatedIn",
                            Expand(OUT, "Supplier", $10,
                                ForeachRelationship(FROM, ":hasSupplier",
                                    Filter(matchData($12.data:string),
                                        Expand(IN, "Item",
                                            ForeachRelationship(TO, ":hasStock",
                                                Expand(OUT, "Stock",
                                                    ForeachRelationship(FROM, ":hasStock",
                                                        Expand(OUT, "OrderLine",
                                                            ForeachRelationship(FROM, ":contains",
                                                                Filter(%sdt <= $6.entry_d:datetime && $6.entry_d:datetime < %edt,
                                                                    Expand(OUT, "Order",
                                                                        ForeachRelationship(FROM, ":hasPlaced",
                                                                            Expand(IN, "Customer",
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
)