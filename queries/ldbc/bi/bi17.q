auto samePerson = [&](auto &v, int idx1, int idx2) {
    return boost::get<node *>(v[idx1])->id() != boost::get<node *>(v[idx2])->id();
});

auto msgInterval = [&](auto &lv, auto &rv) {
    auto msg1_dt = boost::get<ptime>(v[0]);
    auto msg2_dt = boost::get<ptime>(v[5]);
    auto hrs = boost::get<int>(params[1]);
    return (msg1_dt + hours(hrs)) < msg2_dt
});

auto fltrRship1 = [&](auto &v, int idx, string label) {
    if (v[idx].type() == typeid(null_val)) 
        return false;
    auto r = boost::get<relationship *>(v[idx]);
    return r->rship_label == gdb->get_code(label) ? true : true;
});


auto fltrRship2 = [&](auto &v, int idx, string label) {
    if (v[idx].type() == typeid(null_val)) 
        return false;
    auto r = boost::get<relationship *>(v[idx]);
    return r->rship_label == gdb->get_code(label) ? false : true;
});

Sort([$0:uint64 ASC],
    Project([$0.id:uint64, $1],
        GroupBy([$1],
                [sum($1)],
            Union(
                GroupBy([$1],
                        [count($0)],
                    Filter(fltrRship1(tuple, 18, ":hasMember"),
                        RshipExists({2, 17},
                            Expand(OUT, "Person",
                                ForeachRelationship(FROM, ":hasCreator", $13,
                                    Filter($14.name == %tag,
                                        Expand(OUT, "Tag",
                                            ForeachRelationship(FROM, ":hasTag",
                                                Expand(OUT, "Comment",
                                                    ForeachRelationship(FROM, ":replyOf", $4,
                                                        Filter(fltrRship2(tuple, 11, ":hasMember"),
                                                            RshipExists({10, 1},
                                                                Expand(IN, "Forum",
                                                                    ForeachRelationship(TO, ":containerOf", $6,
                                                                        Filter($8.name == %tag,
                                                                            Expand(OUT, "Tag",
                                                                                ForeachRelationship(FROM, ":hasTag", $4,
                                                                                    Filter(msgInterval(tuple),
                                                                                        Project([$2.creationDate:datetime, $6, $8, $10, $12, $12.creationDate:datetime, $14],
                                                                                            Expand(OUT, "Post",
                                                                                                ForeachRelationship(FROM, ":replyOf", {1, 100},
                                                                                                    Expand(IN, "Comment",
                                                                                                        ForeachRelationship(TO, ":hasCreator",
                                                                                                            Filter(samePerson(tuple, 6, 10),
                                                                                                                Expand(OUT, "Person",
                                                                                                                    ForeachRelationship(FROM, ":hasMember",
                                                                                                                        Expand(IN, "Forum",
                                                                                                                            ForeachRelationship(TO, ":containerOf", $4,
                                                                                                                                Expand(OUT, "Person",
                                                                                                                                    ForeachRelationship(FROM, ":hasCreator", $2,
                                                                                                                                        Expand(OUT, "Post",
                                                                                                                                            ForeachRelationship(FROM, ":replyOf", {1, 100},
                                                                                                                                                Expand(IN, "Comment",
                                                                                                                                                    ForeachRelationship(TO, ":hasTag",
                                                                                                                                                        Filter($0.name == %tag,
                                                                                                                                                            NodeScan("Tag")
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
                                            )
                                        )
                                    )
                                )
                            )
                        )
                    )
                ),
                GroupBy([$1],
                        [count($0)],
                    Filter(fltrRship1(tuple, 17, ":hasMember"),
                        RshipExists({2, 16},
                            Expand(OUT, "Person",
                                ForeachRelationship(FROM, ":hasCreator", $12,
                                    Filter($13.name == %tag,
                                        Expand(OUT, "Tag",
                                            ForeachRelationship(FROM, ":hasTag",
                                                Expand(OUT, "Comment",
                                                    ForeachRelationship(FROM, ":replyOf", $4,
                                                        Filter(fltrRship2(tuple, 10, ":hasMember"),
                                                            RshipExists({9, 1},
                                                                Expand(IN, "Forum",
                                                                    ForeachRelationship(TO, ":containerOf", $4,
                                                                        Filter($7.name == %tag,
                                                                            Expand(OUT, "Tag",
                                                                                ForeachRelationship(FROM, ":hasTag", $4,
                                                                                    Filter(msgInterval(tuple),
                                                                                        Project([$2.creationDate:datetime, $6, $8, $10, $12, $12.creationDate:datetime],
                                                                                            Expand(IN, "Post",
                                                                                                ForeachRelationship(TO, ":hasCreator",
                                                                                                    Filter(samePerson(tuple, 6, 10),
                                                                                                        Expand(OUT, "Person",
                                                                                                            ForeachRelationship(FROM, ":hasMember",
                                                                                                                Expand(IN, "Forum",
                                                                                                                    ForeachRelationship(TO, ":containerOf", $4,
                                                                                                                        Expand(OUT, "Person",
                                                                                                                            ForeachRelationship(FROM, ":hasCreator", $2,
                                                                                                                                Expand(OUT, "Post",
                                                                                                                                    ForeachRelationship(FROM, ":replyOf", {1, 100},
                                                                                                                                        Expand(IN, "Comment",
                                                                                                                                            ForeachRelationship(TO, ":hasTag",
                                                                                                                                                Filter($0.name == %tag,
                                                                                                                                                    NodeScan("Tag")
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
                                    )
                                )
                            )
                        )
                    )
                ),
                GroupBy([$1],
                        [count($0)],
                    Filter(fltrRship1(tuple, 18, ":hasMember"),
                        RshipExists({2, 17},
                            Expand(OUT, "Person",
                                ForeachRelationship(FROM, ":hasCreator", $13,
                                    Filter($14.name == %tag,
                                        Expand(OUT, "Tag",
                                            ForeachRelationship(FROM, ":hasTag",
                                                Expand(OUT, "Comment",
                                                    ForeachRelationship(FROM, ":replyOf", $4,
                                                        Filter(fltrRship2(tuple, 11, ":hasMember"),
                                                            RshipExists({10, 1},
                                                                Expand(IN, "Forum",
                                                                    ForeachRelationship(TO, ":containerOf", $6,
                                                                        Filter($8.name == %tag,
                                                                            Expand(OUT, "Tag",
                                                                                ForeachRelationship(FROM, ":hasTag", $4,
                                                                                    Filter(msgInterval(tuple),
                                                                                        Project([$2.creationDate:datetime, $4, $6, $8, $10, $10.creationDate:datetime, $12],
                                                                                            Expand(OUT, "Post",
                                                                                                ForeachRelationship(FROM, ":replyOf", {1, 100},
                                                                                                    Expand(IN, "Comment",
                                                                                                        ForeachRelationship(TO, ":hasCreator",
                                                                                                            Filter(samePerson(tuple, 4, 8),
                                                                                                                Expand(OUT, "Person",
                                                                                                                    ForeachRelationship(FROM, ":hasMember",
                                                                                                                        Expand(IN, "Forum",
                                                                                                                            ForeachRelationship(TO, ":containerOf", $2,
                                                                                                                                Expand(OUT, "Person",
                                                                                                                                    ForeachRelationship(FROM, ":hasCreator",
                                                                                                                                        Expand(IN, "Post",
                                                                                                                                            ForeachRelationship(TO, ":hasTag",
                                                                                                                                                Filter($0.name == %tag,
                                                                                                                                                    NodeScan("Tag")
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
                                    )
                                )
                            )
                        )
                    )
                ),
                GroupBy([$1],
                        [count($0)],
                    Filter(fltrRship1(tuple, 17, ":hasMember"),
                        RshipExists({2, 16},
                            Expand(OUT, "Person",
                                ForeachRelationship(FROM, ":hasCreator", $9,
                                    Filter($13.name == %tag,
                                        Expand(OUT, "Tag",
                                            ForeachRelationship(FROM, ":hasTag",
                                                Expand(OUT, "Comment",
                                                    ForeachRelationship(FROM, ":replyOf", $4,
                                                        Filter(fltrRship2(tuple, 10, ":hasMember"),
                                                            RshipExists({9, 1},
                                                                Expand(IN, "Forum",
                                                                    ForeachRelationship(TO, ":containerOf", $4,
                                                                        Filter($7.name == %tag,
                                                                            Expand(OUT, "Tag",
                                                                                ForeachRelationship(FROM, ":hasTag", $4,
                                                                                    Filter(msgInterval(tuple),
                                                                                        Project([$2, $4, $6, $8, $10, $10.creationDate:datetime],
                                                                                            Expand(IN, "Post",
                                                                                                ForeachRelationship(TO, ":hasCreator",
                                                                                                    Filter(samePerson(tuple, 4, 8),
                                                                                                        Expand(OUT, "Person",
                                                                                                            ForeachRelationship(FROM, ":hasMember",
                                                                                                                Expand(IN, "Forum",
                                                                                                                    ForeachRelationship(TO, ":containerOf", $2,
                                                                                                                        Expand(OUT, "Person",
                                                                                                                            ForeachRelationship(FROM, ":hasCreator",
                                                                                                                                Expand(IN, "Post",
                                                                                                                                    ForeachRelationship(TO, ":hasTag",
                                                                                                                                        Filter($0.name == %tag,
                                                                                                                                            NodeScan("Tag")
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
                            )
                        )
                    )
                )
            )
        )
    )
)