auto isComment = [&](auto res) {
    return pj::has_label(res, "Comment") ?
    query_result(std::string("True")) : query_result(std::string("False"));
};

auto lengthCategory = [&](auto res) {
    auto len = boost::get<int>(res);
    return (len >= 0 && len < 40) ? query_result(std::string("0")) :
            (len >= 40 && len < 80) ? query_result(std::string("1")) :
            (len >= 80 && len < 160) ? query_result(std::string("2")) :
            query_result(std::string("3"));
};

Sort([$0:int DESC, $1:string ASC, $2:string ASC],
    GroupBy([$0, $1, $3], 
            [count($0:int), avg($2:int), sum($2:int), pcount($0:int)],
        Project([year($0.creationDate:datetime), isComment($0), $0.length:int, lengthCategory($0.length:int)],
            Filter($0.creationDate < %date,
                NodeScan(["Post", "Comment"])
            )
        )
    )
)