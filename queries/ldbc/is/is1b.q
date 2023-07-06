Project([$0.firstName:string, $0.lastName:string, $0.birthday:datetime, $0.locationIP:string, $0.browserUsed:string, $2.id:uint64, $0.gender:string, $0.creationDate:datetime],
    Match((p1:Person {id: 933})-[:isLocatedIn]->(p2:Place))
)