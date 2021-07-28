auto filterSum = [&](auto res) {
    auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
    auto s = gdb->get_dictionary()->lookup_code(c);
    std::regex r(".*" + data);
    return std::regex_match(s, r);
});

Sort([$1:int DESC],
    Filter(filterSum(tuple),
        GroupBy([$0], 
                [sum($1:int)],
            Project([$4.id:uint64, $4.order_cnt:int],
                Expand(IN, "Stock",
                    ForeachRelationship(TO, ":hasSupplier",
                        Expand(IN, "Supplier",
                            ForeachRelationship(TO, ":isLocatedIn",
                                Filter($0.name == %name,
                                    NodeScan("Nation")
                                )
                            )
                        )
                    )
                )
            )
        )
    )
)