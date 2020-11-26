# LDBC Benchmark

[The LDBC SNB documentation](http://ldbc.github.io/ldbc_snb_docs/ldbc-snb-specification.pdf)

## Load data
Create and configure a pmem poolset file. Then create a graph pool to contain the graph(s). Note that the pool layout is internally `"poseidon"`. 

```bash
pmempool create --layout="poseidon" obj "/path/to/poolset/file"
```

Load the data of required scale factor:
```bash
./ldbc_loader --import "/path/to/snb/dataset" --pool "/path/to/poolset/file" --db "graphdb_name" --strict
```

Alternatively, create the pool and load a single graph to it at once:
```bash
./ldbc_loader --import "/path/to/snb/dataset" --pool "/path/to/pool" --db "graphdb_name" --strict
```

## Run benchmark
Define the scale factor (e.g. SF1, SF10) in the `ldbc.hpp` file.

To run the Interactive Short Read queries:
```bash
./run_ldbc_reads --pool "/path/to/pool" --db "graphdb_name"
```

To run the Interactive Update queries:
```bash
./run_ldbc_updates --pool "/path/to/pool" --db "graphdb_name"
```

To run the Business Intelligence queries:
```bash
./run_ldbc_bi --pool "/path/to/pool" --db "graphdb_name"
```
