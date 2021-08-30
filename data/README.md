## Test data

The data is taken from https://neo4j.com/developer/guide-import-csv/

Usage:

```
qlc --db TestDB --import nodes:Customer:../data/customers.csv --n4j --import nodes:Product:../data/products.csv --import nodes:Order:../data/orders.csv --import relationships:CONTAINS:../data/order-details.csv --import relationships:ORDERED:../data/ordered.csv
```

## Generic CSV Loader

```
csv_loader --pool /mnt/pmem0/poseidon/TESTDB --db TestDB --format n4j --delimiter , --import nodes:Customer:../data/customers.csv --import nodes:Product:../data/products.csv --import nodes:Order:../data/orders.csv --import relationships:CONTAINS:../data/order-details.csv --import relationships:ORDERED:../data/ordered.csv
```