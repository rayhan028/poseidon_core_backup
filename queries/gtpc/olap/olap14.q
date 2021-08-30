auto filterDeliveryD = [&](auto res) {
    auto dt = (*(reinterpret_cast<const ptime *>(prop.value_)));
    auto edt = %sdt + hours(24 * 30);
    return %sdt <= dt && dt < edt;
});

auto val1 = [&](auto res) {
    auto s = boost::get<std::string>(v[0]);
    std::regex r(%data + ".*");
    return std::regex_match(s, r) ? v[1] : query_result(0.0);
};

auto val2 = [&](auto res) {
    auto sum1 = boost::get<double>(v[0]);
    auto sum2 = boost::get<double>(v[1]);
    auto promo_revenue = (sum2 == 0) ? 0.0 : 100 * sum1 / sum2;
    return query_result(promo_revenue);
};

Project([$2],
    AppendToTuple(val2(tuple),
        GroupBy([], 
                [sum($2:double), sum($1:double)],
            AppendToTuple(val1(tuple),
                Project([$4.data:string, $0.amount:double],
                    Expand(IN, "Item",
                        ForeachRelationship(TO, ":hasStock",
                            Expand(OUT, "Stock",
                                ForeachRelationship(FROM, ":hasStock",
                                    Filter(filterDeliveryD($0.delivery_d:datetime),
                                        NodeScan("OrderLine")
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