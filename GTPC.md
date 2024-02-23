# Running GTPC with Poseidon

1. Download GTPC from https://github.com/dbis-ilm/gtpc

2. Build the data generator of GTPC

```
cd ${GTPC_HOME}/datagen
mkdir build; cd build
cmake ..
make
```
3. Create the data

GTPC data with 5 warehouses:

```
${GTPC_HOME}/datagen/build/gtpc_datagen -d data -w 5
```

4. Import the data in Poseidon

The CSV importer tries to infer the data types of the columns automatically. This doesn't produce
always the best results, but can be controlled by a type specification file, e.g. the following
 `types.mapping` file for GTPC:
```
Customer.phone:string
Supplier.phone:string
```

Using this file you can import the previously generated data:

```
${POSEIDON_HOME}/build/pcli -b 100000 --pool gtpc --db testdb -f ldbc --delimiter '|' \
    --import-path data \
    --import nodes:Customer:customer_0_0.csv \
    --import nodes:District:district_0_0.csv \
    --import nodes:Item:item_0_0.csv \
    --import nodes:Nation:nation_0_0.csv \
    --import nodes:Region:region_0_0.csv \
    --import nodes:Order:order_0_0.csv \
    --import nodes:OrderLine:orderLine_0_0.csv \
    --import nodes:Stock:stock_0_0.csv \
    --import nodes:Supplier:supplier_0_0.csv \
    --import nodes:Warehouse:warehouse_0_0.csv \
    --import relationships:hasPlaced:customer_hasPlaced_order_0_0.csv \
    --import relationships:isLocatedIn:customer_isLocatedIn_nation_0_0.csv \
    --import relationships:serves:district_serves_customer_0_0.csv \
    --import relationships:hasStock:item_hasStock_stock_0_0.csv \
    --import relationships:isPartOf:nation_isPartOf_region_0_0.csv \
    --import relationships:cotains:order_contains_orderLine_0_0.csv \
    --import relationships:hasStock:orderLine_hasStock_stock_0_0.csv \
    --import relationships:hasSupplier:stock_hasSupplier_supplier_0_0.csv \
    --import relationships:isLocatedIn:supplier_isLocatedIn_nation_0_0.csv \
    --import relationships:covers:warehouse_covers_district_0_0.csv \
    --import relationships:hasStock:warehouse_hasStock_stock_0_0.csv \
    --typespec types.mapping
```

This creates a database `testdb` in the pool `gtpc` in the current directory.

5. Run GTPC OLAP queries

The following command expects `testdb/gtpc` in the current directory. The GTPC queries
can be found in `${POSEIDON_HOME}/queries/gtpc/olap`. The file `queries.pql` contains
all queries. Alternatively, the directory contains also all single queries, e.g. `olap1.q`,
`olap2.q` etc.

```
${POSEIDON_HOME}/build/pcli --pool gtpc --db testdb -b 10000 -q queries.pql
```