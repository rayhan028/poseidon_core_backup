import pytest
import os
import sys
sys.path.append('../build')

# import after sys.path is set!
import poseidon

# TODO: should contain more tests of the Python API


def test_create_pool():
    p = poseidon.create_pool("/mnt/pmem0/poseidon/py_test")
    assert p != None
    g = p.create_graph("mygraph")
    assert g != None
    p.close()
    os.remove("/mnt/pmem0/poseidon/py_test")

def test_open_graph():
    p = poseidon.create_pool("/mnt/pmem0/poseidon/py_test")
    assert p != None
    with pytest.raises(RuntimeError):
        g = p.open_graph("mygraph")
    p.close()
    os.remove("/mnt/pmem0/poseidon/py_test")

def test_create_node():
    p = poseidon.create_pool("/mnt/pmem0/poseidon/py_test")
    assert p != None
    g = p.create_graph("mygraph")
    assert g != None
    g.begin()
    a1 = g.create_node("Actor", { "name": "John David Washington"})
    m1 = g.create_node("Movie", { "title": "Tenet"})
    g.commit()
    g.begin()
    anode = g.get_node(a1)
    mnode = g.get_node(m1)
    g.abort()
    p.close()
    os.remove("/mnt/pmem0/poseidon/py_test")