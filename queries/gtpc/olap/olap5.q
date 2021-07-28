auto filterEnrtyD = [&](auto res) {
    auto dt = *(reinterpret_cast<const ptime *>(prop.value_));
    auto edt = %sdt + hours(24 * 365);
    return %sdt <= dt && dt < edt;
});

auto idComp = [&](auto &v) {
    return boost::get<node *>(v[2])->id() == boost::get<node *>(v[14])->id();
});

Sort([$1:double DESC],
    GroupBy([$0], 
            [sum($1:double)],
        Project([$2.name:string, $8.amount:double],
            Filter(idComp(tuple),
                Expand(OUT, "Nation",
                    ForeachRelationship(FROM, ":isLocatedIn",
                        Expand(OUT, "Supplier",
                            ForeachRelationship(FROM, ":hasSupplier",
                                Expand(OUT, "Stock",
                                    ForeachRelationship(FROM, ":hasStock",
                                        Expand(OUT, "OrderLine",
                                            ForeachRelationship(FROM, ":contains",
                                                Filter(filterEnrtyD($6.entry_d:datetime),
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