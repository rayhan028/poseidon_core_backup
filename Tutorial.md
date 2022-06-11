./csv_loader --pool dummy --db testdb -f n4j --import nodes:Movie:../test/movies.csv --import nodes:Actor:../test/actors.csv --import relationships:../test/roles.csv --delimiter ,

./qlc --db testdb -o tt.dot
./qlc --db testdb -s --qmode interp

poseidon> NodeScan()
poseidon> NodeScan('Movie')
poseidon> Filter($0.title == 'Inception (2010)', NodeScan('Movie'))
poseidon> Expand(IN, "Actor", ForeachRelationship(TO, "PLAYED_IN", NodeScan('Movie')))