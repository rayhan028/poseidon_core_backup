# GTPC Benchmark

## Generate graph data
```bash
./gtpc_generator --directory "/output/path/for/generated/files" --warehouses "warehouses"
```

## Run benchmark
```bash
./gtpc_bench --import "/path/to/generated/gtpc/files" --pool "/path/to/pmem/pool" --db "Graph DB name" --olap "OLAP streams" --oltp "OLTP streams"
```