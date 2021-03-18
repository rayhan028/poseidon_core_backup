auto Lambda1 = [&](auto res) {
    pj::has_label(res, "Comment") ?
    std::string("True") : std::string("False");
};

auto Lambda2 = [&](auto res) {
    auto len = boost::get<int>(pj::int_property(res, "length"));
    return (len >= 0 && len < 40) ? query_result(std::string("0")) :
            (len >= 40 && len < 80) ? query_result(std::string("1")) :
            (len >= 80 && len < 160) ? query_result(std::string("2")) :
            query_result(std::string("3"));
};

Sort([$0:DESC, $1:ASC, $2:ASC],
    Aggregate([$0:COUNT, $2:AVG, $2:SUM, $0:PCOUNT],
        Group([$0, $1, $3],
            Project([$0.creationDate:datetime, $0.Lambda1, $0.length:int, $0.Lambda2],
                Filter($0.creationDate < %date,
                    NodeScan(["Post", "Comment"])
                )
            )
        )
    )
)