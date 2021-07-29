auto knowsRship = [&](auto &v, int idx, string label) {
    if (boost::get<node *>(v[0])->id() == boost::get<node *>(v[4])->id())
    return false;
    relationship *r = v[5].type() == typeid(relationship *) ?
    boost::get<relationship *>(v[5]) : v[6].type() == typeid(relationship *) ?
    boost::get<relationship *>(v[6]) : nullptr;
    return !r ? true : r->rship_label == gdb->get_code(":knows") ?
    false : true;
});

Limit(20,
    Sort([$1:uint64 DESC, $0:uint64 ASC],
        Project([$0.id:uint64, $1],
            GroupBy([$4],
                    [count($0)],
                Filter($6.name:string == %tag,
                    Expand(OUT, "Tag",
                        ForeachRelationship(FROM, ":hasInterest", $4,
                            RshipExists({4, 0},
                                RshipExists({0, 4},
                                    ForeachRelationship(ALL, ":knows",
                                        Filter($0.id == %id,
                                            NodeScan("Person")
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