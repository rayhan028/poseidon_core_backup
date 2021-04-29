auto samePerson = [&](auto res1, auto res2) {
    return boost::get<uint64_t>(res1) == boost::get<uint64_t>(res2);
};

auto distinctBag = [&](auto &v) {
    auto a = boost::get<node *>(v[0])->id();
    auto b = boost::get<node *>(v[1])->id();
    auto c = boost::get<node *>(v[2])->id();
    if (a > c)
        std::swap(a, c);
    if (a > b)
        std::swap(a, b);
    if (b > c)
        std::swap(b, c);
    std::string key = std::to_string(a) + "_" + std::to_string(b) + "_" + std::to_string(c);
    return query_result(key);
});

Count(
    GroupBy([$5],
        AppendToTuple(distinctBag(tuple),
            Filter(samePerson($4.id:uint64, $18.id:uint64),
                Filter($17.creationDate > %date,
                    ForeachRelationship(ALL, ":knows", $12,
                        Filter($16.name == %country,
                            Expand(OUT, "Place",
                                ForeachRelationship(FROM, ":isPartOf",
                                    Expand(OUT, "Place",
                                        ForeachRelationship(FROM, ":isLocatedIn",
                                            Filter($11.creationDate > %date,
                                                ForeachRelationship(ALL, ":knows", $6,
                                                    Filter($10.name == %country,
                                                        Expand(OUT, "Place",
                                                            ForeachRelationship(FROM, ":isPartOf",
                                                                Expand(OUT, "Place",
                                                                    ForeachRelationship(FROM, ":isLocatedIn",
                                                                        Filter($5.creationDate > %date,
                                                                            ForeachRelationship(ALL, ":knows",
                                                                                Expand(IN, "Person",
                                                                                    ForeachRelationship(TO, ":isLocatedIn",
                                                                                        Expand(IN, "Place",
                                                                                            ForeachRelationship(TO, ":isPartOf",
                                                                                                Filter($0.name == %country,
                                                                                                    NodeScan("Place")
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
)