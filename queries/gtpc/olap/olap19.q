auto matchData1 = [&](auto res) {
    auto s = boost::get<std::string>(v[1]);
    std::regex r(".*" + %data1);
    auto pred1 = std::regex_match(s, r);
    auto p = boost::get<double>(v[2]);
    auto pred2 = %price1 <= p && p <= %price2;
    return pred1 && pred2;
});

auto matchData2 = [&](auto res) {
    auto s = boost::get<std::string>(v[1]);
    std::regex r(".*" + %data2);
    auto pred1 = std::regex_match(s, r);
    auto p = boost::get<double>(v[2]);
    auto pred2 = %price1 <= p && p <= %price2;
    return pred1 && pred2;
});

auto matchData3 = [&](auto res) {
    auto s = boost::get<std::string>(v[1]);
    std::regex r(".*" + %data3);
    auto pred1 = std::regex_match(s, r);
    auto p = boost::get<double>(v[2]);
    auto pred2 = %price1 <= p && p <= %price2;
    return pred1 && pred2;
});

auto filterAmt = [&](auto res) {
    auto amount = boost::get<int>(v[0]);
    return %quantity1 <= amount && amount <= %quantity2;
});

GroupBy([], 
        [sum($0:int)],
    Union(
        Filter(filterAmt(tuple),
            Project([$6.quantity:int],
                Expand(IN, "OrderLine",
                    ForeachRelationship(TO, ":hasStock",
                        Expand(OUT, "Stock",
                            ForeachRelationship(FROM, ":hasStock", $0,
                                Filter(matchData2(tuple),
                                    Project([$0, $0.data:string, $0.price:double],
                                        NodeScan("Item")
                                    )
                                )
                            )
                        )
                    )
                )
            )
        ),
        Filter(filterAmt(tuple),
            Project([$6.quantity:int],
                Expand(IN, "OrderLine",
                    ForeachRelationship(TO, ":hasStock",
                        Expand(OUT, "Stock",
                            ForeachRelationship(FROM, ":hasStock", $0,
                                Filter(matchData2(tuple),
                                    Project([$0, $0.data:string, $0.price:double],
                                        NodeScan("Item")
                                    )
                                )
                            )
                        )
                    )
                )
            )
        ),
        Filter(filterAmt(tuple),
            Project([$6.quantity:int],
                Expand(IN, "OrderLine",
                    ForeachRelationship(TO, ":hasStock",
                        Expand(OUT, "Stock",
                            ForeachRelationship(FROM, ":hasStock", $0,
                                Filter(matchData1(tuple),
                                    Project([$0, $0.data:string, $0.price:double],
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