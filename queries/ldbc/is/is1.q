Project([$0.firstName:string, $0.lastName:string, udf::getDate($0:qresult), $0.locationIP:string, $0.browserUsed:string, $2.id:uint64, $0.gender:string, $0.creationDate:datetime],
  Expand(OUT, "Place",
    ForeachRelationship(FROM, ":isLocatedIn", 
      Filter($0.id == 933, 
        NodeScan("Person")
      )
    )
  )
)