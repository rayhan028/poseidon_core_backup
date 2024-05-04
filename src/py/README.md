### Poseidon Python API

In addition, there is a thin wrapper library for providing a Python API. This API allows creating and opening graph databases, as well as creating and accessing nodes and relationships.

In the following Python example the database `imdb` is created.

```python
import poseidon

# create a new graph pool and a graph database
pool = poseidon.create_pool("my_pool")
graph = pool.create_graph("imdb", 1000)
```

Next, we create a few nodes and relationships that are stored in the database.

```python
graph.begin()
a1 = graph.create_node("Actor", { "name": "John David Washington"})
a2 = graph.create_node("Actor", { "name": "Elizabeth Debicki"})
m1 = graph.create_node("Movie", { "title": "Tenet"})
graph.create_relationship(a1, m1, ":plays_in", {})
graph.create_relationship(a2, m1, ":plays_in", {})
graph.commit()
```

Nodes and relationships can be retrieved by their id. In addition, for a given node all its relationships can be obtained.

```python
graph.begin()
n = graph.get_node(m1)
for r in graph.get_to_relationships(n.id):
    print(r, graph.get_node(r.from_node), "-->", graph.get_node(r.to_node))
graph.commit()
```

Furthermore, access to id, label and properties of nodes and relationships is also possible:

```python
print(n.id, n.label)
print(n["title"])
```

Poseidon queries are also supported:

```python
res = graph.query("NodeScan('Actor')")
for tup in res:
    print(tup)
```