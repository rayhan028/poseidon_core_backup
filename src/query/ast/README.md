# AST Nodes

During query compilation we build an abstract syntax tree that is used to compile the query into LLVM IR code
or to build a query expression as ahead-of-time (AOT) compilation. The following table shows the various node
types which are all represent by the class `ast_op`.

`op_type_`     | Semantics | Param #1 | Param #2 | Param #3 | Param #4 | Param #5
---------------|-----------|----------|----------|----------|----------|----------
node_scan      | Scans the list of nodes (either on the nodes table or via index) and returns all nodes with the given type (label). | type (label) of nodes (`string`) | 
filter         | Selects all tuples (nodes, relationships) which satisfy the given filter condition. | filter condition (`expr`) | | 
foreach_rship  | Traverses all relationships for each input node. | direction of the relationship (`TO`, `FROM` as `string`) | relationship type (label) to traverse (`string`) | 
expand         | Returns all neighbor (connected) nodes for each input relationship. | role of the connected node (`IN`, `OUT` as `string`) | node type (label) to follow  (`string`) |
project        | Applies a projection to the input stream. | list of projection expression where an expression is a struct of name (`pname`) in the form `$0.name` and data type (`ptype`) | |
limit          | Limits the result set to the first n elements. | maximum number of results (`int`) | |
sort           |
group_by       |
hash_join      |
leftouter_join | Performs a left outer join of both input streams by applying the given join condition. | join condition (`expr`) | |
create_node    | Creates a new node either from the given data or the input stream specified by a node pattern. | node label (`string`) | list of node properties (`jproperty_list`) 
create_rship   | Creates a new relationship either from the given data or the input stream specified by a relationship pattern. | direction (`->`, `<-`. `<->`) | position of the first node in the input tuple (`int`) | position of the first node in the input tuple (`int`) | relationship label (`string`) | list of relationship properties (`jproperty_list`)