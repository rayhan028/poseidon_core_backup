# isComment
auto Lambda1 = [&](auto res) {
    pj::has_label(res, "Comment") ?
    std::string("True") : std::string("False");
};

# lengthCategory
auto Lambda2 = [&](auto res) {
    auto len = boost::get<int>(pj::int_property(res, "length"));
    return (len >= 0 && len < 40) ? query_result(std::string("0")) :
            (len >= 40 && len < 80) ? query_result(std::string("1")) :
            (len >= 80 && len < 160) ? query_result(std::string("2")) :
            query_result(std::string("3"));
};

Sort([$0:int DESC, $1:int ASC, $2:int ASC],
        GroupBy([$0, $1, $3], 
                [count($4:int), avg($2:int), sum($3:int), pcount($3:int)],
            Project([year($0.creationDate:datetime), isComment($0), $0.length:int, lengthCategory($0.length:int), $0.id],
                Filter($0.creationDate < %date,
                    NodeScan(["Post", "Comment"])
                )
            )
        )
    )
)