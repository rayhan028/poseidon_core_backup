auto filterPh1 = [&](auto res) {
    auto pred1 = boost::get<double>(v[0]) < 0.0;
    auto phone = boost::get<uint64_t>(v[1]);
    auto str = std::to_string(phone).substr(1, 1);
    auto pred2 = (str == %ph[0]) || (str == %ph[1]) ||
                    (str == %ph[2]) || (str == %ph[3]) ||
                    (str == %ph[4]) || (str == %ph[5]) ||
                    (str == %ph[6]);
    return pred1 && pred2;
});

auto filterPh2 = [&](auto res) {
    auto pred1 = boost::get<double>(v[1]) >= boost::get<double>(v[3]);
    auto phone = boost::get<uint64_t>(v[2]);
    auto str = std::to_string(phone).substr(1, 1);
    auto pred2 = (str == %ph[0]) || (str == %ph[1]) ||
                    (str == %ph[2]) || (str == %ph[3]) ||
                    (str == %ph[4]) || (str == %ph[5]) ||
                    (str == %ph[6]);
    return pred1 && pred2;
});

auto joinPred = [&](auto &lv, auto &rv) {
    return boost::get<node *>(lv[0])->id() == boost::get<node *>(rv[0])->id();
});

auto filterEqual = [&](auto res) {
    return v[4].type() != typeid(null_val);
});

auto subStr = [&](auto res) {
    auto str = boost::get<std::string>(pj::string_property(res, "state"));
    auto sub = str.substr(1, 1);
    return query_result(sub);
});

Sort([$0:string ASC],
    GroupBy([$0], 
            [count($0), sum($1:double)],
        Project([subStr($0), $0.balance:double],
            Filter(filterEqual(tuple),
                Outerjoin(joinPred(tuple1, tuple2),
                    Filter(filterPh(tuple),
                        Crossjoin(
                            Project([$0, $0.balance:double, $0.phone:uint64],
                                NodeScan("Customer")
                            ),
                            GroupBy([], 
                                    [avg($0:double)],
                                Filter(filterPh1(tuple),
                                    Project([$0.balance:double, $0.phone:uint64],
                                        NodeScan("Customer")
                                    )
                                )
                            )
                        )
                    ),
                    Expand(OUT, "Order",
                        ForeachRelationship(FROM, ":hasPlaced", $0,
                            Filter(filterPh(tuple),
                                Crossjoin(
                                    Project([$0, $0.balance:double, $0.phone:uint64],
                                        NodeScan("Customer")
                                    ),
                                    GroupBy([], 
                                            [avg($0:double)],
                                        Filter(filterPh1(tuple),
                                            Project([$0.balance:double, $0.phone:uint64],
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
)