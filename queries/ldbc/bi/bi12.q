auto hasContent1 = [&](auto res) {
    return pj::has_property(res, "content") ?
    query_result(std::string("1")) : query_result(std::string("0"));
};

auto hasContent2 = [&](auto res) {
    return pj::has_property(res, "content") ?
    pj::string_property(res, "language") : query_result("n/a");
};

auto postFeatures = [&](auto &v) {
    if (boost::get<int>(v[1]) == 0 ||
        boost::get<int>(v[2]) >= boost::get<int>(params[1]) ||
        boost::get<ptime>(v[3]) <= boost::get<ptime>(params[0]))
            return false;
    for (std::size_t i = 2; i < params.size(); i++)
        if (boost::get<std::string>(v[4]) == boost::get<std::string>(params[i]))
            return true;
    return false;
});

auto cmtFeatures = [&](auto &v) {
    if (boost::get<int>(v[2]) == 0 ||
        boost::get<int>(v[3]) >= boost::get<int>(params[1]) ||
        boost::get<ptime>(v[4]) <= boost::get<ptime>(params[0]))
            return false;
    else
        return true;
});

auto joinPred = [&](auto &lv, auto &rv) {
    return boost::get<node *>(lv[0])->id() == boost::get<node *>(rv[0])->id();
});

auto appendSum = [&](auto &v) {
    return v[1].type() == typeid(null_val) ?
        query_result((uint64_t)0) : query_result(boost::get<uint64_t>(v[2]));
};

Sort([$1:uint64 DESC, $0:uint64 DESC],
    GroupBy([$3],
            [count($0)],
        AppendToTuple(appendSum(tuple),
            Outerjoin(joinPred(tuple1, tuple2),
                GroupBy([$0],
                        [sum($1)],
                    Union(
                        GroupBy([$0],
                                [count($0)],
                            Filter($6.language:string IN %languages
                                Expand(OUT, "Post",
                                    ForeachRelationship(FROM, ":replyOf", {1, 100}, $1,
                                        Filter(postFeatures(tuple),
                                            Project([$0, $2, hasContent1($2), $2.length:int, $2.creationDate:datetime],
                                                Expand(IN, "Comment",
                                                    ForeachRelationship(TO, ":hasCreator",
                                                        NodeScan("Person")
                                                    )
                                                )
                                            )
                                        )
                                    )
                                )
                            )
                        ),
                        GroupBy([$0],
                                [count($0)],
                            Filter(postFeatures(tuple),
                                Project([$0, hasContent1($2), $2.length:int, $2.creationDate:datetime, hasContent2($2)],
                                    Expand(IN, "Post",
                                        ForeachRelationship(TO, ":hasCreator",
                                            NodeScan("Person")
                                        )
                                    )
                                )
                            )
                        )
                    )
                ),
                NodeScan("Person")
            )
        )
    )
)