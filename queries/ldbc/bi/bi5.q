auto score = [&](auto &v) {
    auto reply_cnt = boost::get<uint64_t>(v[1]);
    auto like_cnt = boost::get<uint64_t>(v[3]);
    auto msg_cnt = boost::get<uint64_t>(v[5 ]);
    auto score = msg_cnt + 2 * reply_cnt + 10 * like_cnt;
    return query_result(score);
};

Limit(100, 
    Sort([$4:uint64 DESC, $0:uint64 ASC],
        Project([$0.id:uint64, $1, $3, $5, $6],
            AppendToTuple(score(tuple),
                Join(HASHJOIN_ON_NODE, {$0, $0},
                    GroupBy([$4],
                            [count($0)],
                        Expand(IN, "Comment",
                            ForeachRelationship(TO, ":replyOf", $2,
                                Expand(OUT, "Person",
                                    ForeachRelationship(FROM, ":hasCreator",
                                        Expand(IN, ["Post", "Comment"],
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
                    ),
                    Join(HASHJOIN_ON_NODE, {$0, $0},
                        GroupBy([$4],
                                [count($0)],
                            Expand(IN, "Person",
                                ForeachRelationship(TO, ":likes", $2,
                                    Expand(OUT, "Person",
                                        ForeachRelationship(FROM, ":hasCreator",
                                            Expand(IN, ["Post", "Comment"],
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
                        ),
                        GroupBy([$4],
                                [count($0)],
                            Expand(OUT, "Person",
                                ForeachRelationship(FROM, ":hasCreator",
                                    Expand(IN, ["Post", "Comment"],
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