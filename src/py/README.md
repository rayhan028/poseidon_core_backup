### Poseidon Python API

In addition, there is a thin wrapper library for providing a Python API. This API allows creating and opening graph databases, as well as creating and accessing nodes and relationships.

In the following Python example the database `imdb` is created.

```python
import poseidon

# create a new graph pool and a graph database
pool = poseidon.create_pool("my_pool")
graph = pool.create_graph("imdb")
```

Next, we create a few nodes and relationships that are stored in the database.

```python
graph.begin()
a1 = graph.create_node("Actor", { "name": "John David Washington"})
a2 = graph.create_node("Actor", { "name": "Elizabeth Debicki"})
m1 = graph.create_node("Movie", { "title": "Tenet"})
graph.create_relationship(":plays_in", a1, m1)
graph.create_relationship(":plays_in", a2, m1)
graph.commit()
```

Nodes and relationships can be retrieved by their id. In addition, for a given node all its relationships can be obtained.

```python
graph.begin()
n = graph.node_by_id(m1)
for r in n.get_relationships():
    print(r.from_node(), r.to_node())
```