Sort([$0:int DESC, $1:string ASC, $2:string ASC], 
    GroupBy([$0:datetime, $1:int, $3:int], [count($0:int), avg($2:int), sum($2:int), count($0:int)], 
        Project([udf::year($0.creationDate:datetime), udf::isComment($0:node), $0.length:int, udf::lengthCategory($0.length:int)], 
            Filter($0.creationDate < %date, 
                NodeScan("Post", "Comment") 
))))