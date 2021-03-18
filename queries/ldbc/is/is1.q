Project([$0.firstName:string, $0.lastName:string, $0.birthday:datetime, $0.locationIP:string, $0.browserUsed:string, $2.id:uint64, $0.gender:string, $0.creationDate:datetime],
  Expand(OUT, "Place",
    ForeachRelationship(FROM, ":isLocatedIn", 
      Filter($0.id == 42, 
        NodeScan("Person")
      )
    )
  )
)
