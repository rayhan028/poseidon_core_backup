auto matchData = [&](auto res) {
    auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
    auto s = gdb->get_dictionary()->lookup_code(c);
    std::regex r(".*" + %data);
    return !std::regex_match(s, r);
});

auto filterQtty = [&](auto res) {
    auto qty = boost::get<int>(v[0]);
    auto avg_qty = boost::get<double>(v[2]);
    return qty <= avg_qty;
});

auto yearlyAvg = [&](auto res) {
    auto sum = boost::get<double>(v[0]);
    auto yearly_avg = sum / (double)2.0;
    return query_result(yearly_avg);
});

Project([$1],
    AppendToTuple(yearlyAvg(tuple),
        GroupBy([], 
                [sum($1:double)],
            Filter(filterQtty(tuple),
                Project([$5.quantity:int, $5.amount:double, $1],
                    Expand(IN, "OrderLine",
                        ForeachRelationship(TO, ":hasStock",
                            Expand(OUT, "Stock",
                                ForeachRelationship(FROM, ":hasStock", $0,
                                    GroupBy([$0], 
                                            [avg($1:int)],
                                        Project([$0, $4.quantity:int],
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
)