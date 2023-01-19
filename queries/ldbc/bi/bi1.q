Project([$0:int, $1:int, $2:int, $3:int, $4:int, $5:int, $7:double],
    Sort([$0:int DESC, $1:int ASC, $2:int ASC], 
        Append(udf::percentageCount($3:int, $6:int),
            CrossJoin(
                GroupBy([$0:int, $1:int, $3:int], [count($0:int), avg($2:int), sum($2:int)], 
                    Project([udf::year($0.creationDate:datetime), udf::isComment($0:qresult), $0.length:int, udf::lengthCategory($0.length:int)], 
                        Filter($0.creationDate < %date, 
                            NodeScan("Post", "Comment")
                        )
                    )
                ),
                Aggregate([count($0:node)], 
                    Project([udf::year($0.creationDate:datetime), udf::isComment($0:qresult), $0.length:int, udf::lengthCategory($0.length:int)], 
                        Filter($0.creationDate < %date, 
                            NodeScan("Post", "Comment")
                        )
                    )
                )
            )
        )
    )
)