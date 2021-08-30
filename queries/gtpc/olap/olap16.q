auto matchData = [&](auto res) {
    auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
    auto s = gdb->get_dictionary()->lookup_code(c);
    std::regex r(%data + ".*");
    return !std::regex_match(s, r);
});

auto matchComment = [&](auto res) {
    auto c = *(reinterpret_cast<const dcode_t *>(prop.value_));
    auto s = gdb->get_dictionary()->lookup_code(c);
    std::regex r(".*" + %comment + ".*");
    return !std::regex_match(s, r);
});

auto brandSub = [&](auto res) {
    auto str = boost::get<std::string>(pj::string_property(res, "data"));
    auto brand = str.substr(1, 3);
    return query_result(brand);
});

Sort([$3:uint64 DESC],
    GroupBy([$0, $1, $2], 
            [count($0)],
        Distinct(
            Project([$0.name:string, brandSub($0), $0.price:double, $4.id:uint64],
                Filter(matchComment($0.data:string),
                    Expand(OUT, "Supplier",
                        ForeachRelationship(FROM, ":hasSupplier",
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