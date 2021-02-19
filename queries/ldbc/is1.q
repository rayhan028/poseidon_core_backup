Project([$0.firstName:string, $0.lastName:string,
                            $0.birthday:date, $0.locationIP:string, 
                            $0.browserUsed:string, $2.id:uint64,
                            $0.gender:string, $0.creationDate:time],
  ForeachRelationship(FROM, ":isLocatedIn", 
    Filter($0.id == %personId, 
      NodeScan("Person")
    )
  )
)

