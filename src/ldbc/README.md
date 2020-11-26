# LDBC Benchmark

[The LDBC SNB documentation](http://ldbc.github.io/ldbc_snb_docs/ldbc-snb-specification.pdf)

## Load data
Create and configure a pmem poolset file. Then create a graph pool to contain the graph(s). Note that the pool layout is internally "poseidon". 

```bash
pmempool create --layout="poseidon" obj "/path/to/poolset/file"

```

Load the data of required scale factor:
```bash
./ldbc_loader -i "/path/to/snb/dataset" -p "/path/to/poolset/file" -d "graphdb_name" --strict

```

Alternatively, create the pool and load a single graph to it at once:
```bash
./ldbc_loader -i "/path/to/snb/dataset" -p "/path/to/pool" -d "graphdb_name" --strict

```

## Run benchmark
Define the scale factor (e.g. SF1, SF10) in the ldbc.hpp file.

To run the Interactive Short Read queries:
```bash
./run_ldbc_reads -p "/path/to/pool" -d "graphdb_name"

```

To run the Interactive Update queries:
```bash
./run_ldbc_updates -p "/path/to/pool" -d "graphdb_name"

```

To run the Business Intelligence queries:
```bash
./run_ldbc_bi -p "/path/to/pool" -d "graphdb_name"

```
