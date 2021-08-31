auto avgMsgs = [&](auto &v) {
    auto sdt = to_iso_extended_string(boost::get<ptime>(v[2]));
    auto ssyr = sdt.substr(0, sdt.find("-"));
    auto ssmo = sdt.substr(5, 2);
    auto syr = std::stoi(ssyr);
    auto smo = std::stoi(ssmo);
    auto edt = to_iso_extended_string(boost::get<ptime>(params[1]));
    auto esyr = edt.substr(0, edt.find("-"));
    auto esmo = edt.substr(5, 2);
    auto eyr = std::stoi(esyr);
    auto emo = std::stoi(esmo);
    auto msgs = boost::get<uint64_t>(v[1]);
    uint64_t months =
        (syr == eyr) ? (emo - smo + 1) : (12 - smo + 1) + ((eyr - syr) * 12) + emo;
    uint64_t avg_msg = msgs / months;
    return query_result(avg_msg);
};

auto joinPred = [&](auto &lv, auto &rv) {
    return boost::get<node *>(lv[0])->id() == boost::get<node *>(rv[0])->id();
});

auto zmbMsgCnt = [&](auto &v) {
    return v[4].type() == typeid(null_val) ? true :
            boost::get<uint64_t>(v[4]) < 1 ? true : false;
});

auto appendScore1 = [&](auto &v, int idx) {
    return v[idx].type() == typeid(null_val) ?
        query_result((uint64_t)0) : query_result(boost::get<uint64_t>(v[idx]));
});

auto appendScore2 = [&](auto &v) {
    double score = boost::get<uint64_t>(v[6]) == 0 ? 0.0 :
                    boost::get<uint64_t>(v[5]) / (double)boost::get<uint64_t>(v[6]); 
    return query_result(score);
});

Limit(100,
    Sort([$3:double DESC, $0:uint64 ASC],
        Project([$0.id:uint64, $5, $6, $7],
            AppendToTuple(appendScore2(tuple),
                AppendToTuple(appendScore1(tuple, 4),
                    AppendToTuple(appendScore1(tuple, 2),
                        Outerjoin(joinPred(tuple1, tuple2),
                            Outerjoin(joinPred(tuple1, tuple2),
                                Project([$0],
                                    Filter(zmbMsgCnt(tuple),
                                        Outerjoin(joinPred(tuple1, tuple2),
                                            Project([$4],
                                                Filter($4.creationDate < %date,
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
                                            ),
                                            AppendToTuple(avgMsgs(tuple),
                                                Project([$0, $1, $0.creationDate:datetime],
                                                    GroupBy([$4],
                                                            [count($0)],
                                                        Filter($6.creationDate < %date,
                                                            Expand(IN, ["Post", "Comment"],
                                                                ForeachRelationship(TO, ":hasCreator",
                                                                    Filter($4.creationDate < %date,
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
                                ),
                                Join(HASHJOIN_ON_NODE, {$4, $0},
                                    Expand(IN, "Person",
                                        ForeachRelationship(TO, ":likes",
                                            Expand(IN, ["Post", "Comment"],
                                                ForeachRelationship(TO, ":hasCreator",
                                                    Project([$0],
                                                        Filter(zmbMsgCnt(tuple),
                                                            Outerjoin(joinPred(tuple1, tuple2),
                                                                Project([$4],
                                                                    Filter($4.creationDate < %date,
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
                                                                ),
                                                                AppendToTuple(avgMsgs(tuple),
                                                                    Project([$0, $1, $0.creationDate:datetime],
                                                                        GroupBy([$4],
                                                                                [count($0)],
                                                                            Filter($6.creationDate < %date,
                                                                                Expand(IN, ["Post", "Comment"],
                                                                                    ForeachRelationship(TO, ":hasCreator",
                                                                                        Filter($4.creationDate < %date,
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
                                    ),
                                    Project([$0],
                                        Filter(zmbMsgCnt(tuple),
                                            Outerjoin(joinPred(tuple1, tuple2),
                                                Project([$4],
                                                    Filter($4.creationDate < %date,
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
                                                ),
                                                AppendToTuple(avgMsgs(tuple),
                                                    Project([$0, $1, $0.creationDate:datetime],
                                                        GroupBy([$4],
                                                                [count($0)],
                                                            Filter($6.creationDate < %date,
                                                                Expand(IN, ["Post", "Comment"],
                                                                    ForeachRelationship(TO, ":hasCreator",
                                                                        Filter($4.creationDate < %date,
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
                            ),
                            GroupBy([$0],
                                    [count($0)],
                                Expand(IN, "Person",
                                    ForeachRelationship(TO, ":likes",
                                        Expand(IN, ["Post", "Comment"],
                                            ForeachRelationship(TO, ":hasCreator",
                                                Project([$0],
                                                    Filter(zmbMsgCnt(tuple),
                                                        Outerjoin(joinPred(tuple1, tuple2),
                                                            Project([$4],
                                                                Filter($4.creationDate < %date,
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
                                                            ),
                                                            AppendToTuple(avgMsgs(tuple),
                                                                Project([$0, $1, $0.creationDate:datetime],
                                                                    GroupBy([$4],
                                                                            [count($0)],
                                                                        Filter($6.creationDate < %date,
                                                                            Expand(IN, ["Post", "Comment"],
                                                                                ForeachRelationship(TO, ":hasCreator",
                                                                                    Filter($4.creationDate < %date,
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
    )
)