Union(
    Project([$4.id:uint64, $4.title:string, $6.id:uint64, $6.firstName:string, $6.lastName:string], 
        Match((c:Comment { id: 561 })-[:replyOf*1..100]->(po:Post)<-[:containerOf]-(f:Forum)-[:hasModerator]->(p:Person)))                       
    ),
    Project([$2.id:uint64, $2.title:string, $4.id:uint64, $4.firstName:string, $4.lastName:string], 
        Match((po:Post { id: 233 })<-[:containerOf]-(f:Forum)-[:hasModerator]->(p:Person))              
    )
)