/* This query lists the market share of a given nation for customers from a certain region in which 
   kinds of items are "produced". */
Sort([$0:int ASC],
     GroupBy([$0:int],[sum($1:double)],
          Project([pb::year($2.entry_d:datetime), Case($16.name:string == 'ITALY', $4.amount:double, 0.0)],
               Expand(OUT, 'Nation',
                    ForeachRelationship(FROM, 'isLocatedIn', $14,
                         Expand(OUT, 'Supplier',
                              ForeachRelationship(FROM, 'hasSupplier', $6,
                                   Filter($12.name:string == 'EUROPE',
                                        Expand(OUT, 'Region',
                                             ForeachRelationship(FROM, 'isPartOf', $10,
                                                  Expand(OUT, 'Nation',
                                                       ForeachRelationship(FROM, 'isLocatedIn', $0,
                                                            Filter($8.id:uint64 < 1000 && $8.data:string =~ '.*b' && $2.entry_d:datetime >= pb::to_datetime('2007-01-02 00:00:00.000000') && $2.entry_d:datetime < pb::to_datetime('2012-01-02 00:00:00.000000'),
                                                                 Match((c:Customer)-[:hasPlaced]->(o:Order)-[:contains]->(ol:OrderLine)-[:hasStock]->(s:Stock)<-[:hasStock]-(i:Item))
                                                            )
                                                       )
                                                  )
                                             )
                                        )
                                   )
                              )        
                         )
                    )
               )
          )
     )
)
