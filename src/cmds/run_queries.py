#!/usr/bin/env python3.12
import os
import sys
import click
import pathlib
from pathlib import Path
from tabulate import tabulate
import random
import timeit

lib_path = pathlib.Path(__file__).parent.parent.parent.resolve() / 'build'
if os.path.isdir(lib_path):
    sys.path.append(str(lib_path))
else:
    print(f"Cannot infer lib_path {lib_path}")

# sys.path.append('/Users/kai/Projects/poseidon_core/build')

# import after sys.path is set!
import poseidon

@click.command()
@click.option('--pool', '-p', help="Path to the PMem/file pool")
@click.option('--db', '-d', help="Database name (required)")
@click.option('--buffersize', '-b', default=5000, help="Size of the bufferpool (in pages)")
@click.option('--query-dir', '-q', help="Directory with query files (required)")
@click.option('--num', '-n', default=100, help="Number of queries to be executed randomly")

def main(pool, db, buffersize, query_dir, num):
    p = poseidon.open_pool(pool, 1024 * 1024 * 80)
    g = p.open_graph(db, buffersize)

    query_list, files = read_queries(query_dir) 

    # run_random(g, query_list, num)
    qtimes = run_profile(g, query_list)
    for i in range(0, len(query_list)):
        print(f"{files[i]}: {qtimes[i]}")
    
    p.close()

def run_profile(g, query_list):
    query_times = [0] * len(query_list)
    for i in range(0, 3):
        for q in range(0, len(query_list)):
            qstr = query_list[q]
            tm = timeit.timeit(lambda: run_query(g, q, qstr), number=1)    
            query_times[q] += tm

    for q in range(0, len(query_list)):
        query_times[q] /= 10

    return query_times
        
def run_random(g, query_list, num):
    for i in range(0,num):
        qstr = random.choice(query_list)   
        print(qstr)
        run_query(g, i, qstr)

def read_query_from_file(fname):
    txt = Path(fname).read_text()    
    return txt.replace('\n', '')

def read_queries(dir):
    query_list = []
    file_names = []
    p = Path(dir)
    fnames = list(p.glob('**/*.q'))
    for file in fnames:
        qstr = read_query_from_file(str(file))
        file_names.append(str(file))
        query_list.append(qstr)
    return query_list, file_names

def run_query(g, num, qstr):
    print(f"RUN #{num}: {qstr}")
    try:
        res = g.query(qstr)

        # print(tabulate(res, tablefmt="simple_grid", maxcolwidths=20))

    except EOFError:
        print("EOF")
    except RuntimeError as e:
        print(f"ERROR: '{str(e)}'")
 

if __name__ == "__main__":
    main()
