auto timePeriod1 = [&](auto res) {
    auto d = boost::get<ptime>(res);
    time_period duration(%date, hours(24*100));
    return duration.contains(d) ? true : false;
};

auto timePeriod2 = [&](auto res) {
    auto d = boost::get<ptime>(res);
    time_period duration1(%date, hours(24*100));
    auto dt2 = duration1.last();
    time_period duration2(dt2, hours(24*100));
    return duration2.contains(d) ? true : false;
};

auto countDiff = [&](auto &v) {
    auto cnt = boost::get<uint64_t>(v[1]);
    auto nxt_cnt = boost::get<uint64_t>(v[2]);
    uint64_t diff = cnt > nxt_cnt ? cnt - nxt_cnt : nxt_cnt - cnt;
    return query_result(diff);
});

Limit(100, 
    Sort([$3:uint64 DESC, $0:string ASC],
        AppendToTuple(countDiff(tuple),
            Project([$0.name:string, $1, $3],
                Hashjoin({$0, $0},
                    GroupBy([$2],
                            [count($0)],
                        Expand(OUT, "Tag",
                            ForeachRelationship(FROM, ":hasTag",
                                Filter(timePeriod1($0.creationDate:datetime),
                                    NodeScan(["Post", "Comment"])
                                )
                            )
                        )
                    ),
                    GroupBy([$2],
                            [count($0)],
                        Expand(OUT, "Tag",
                            ForeachRelationship(FROM, ":hasTag",
                                Filter(timePeriod2($0.creationDate:datetime),
                                    NodeScan(["Post", "Comment"])
                                )
                            )
                        )
                    )
                )
            )
        )
    )
)