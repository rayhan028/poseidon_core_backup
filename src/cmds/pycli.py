#!/usr/bin/env python3.12
import os
import sys
import click
import pathlib
from prompt_toolkit import PromptSession
from prompt_toolkit.formatted_text import HTML
from prompt_toolkit.history import FileHistory

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

def main(pool, db, buffersize):
    p = poseidon.open_pool(pool, 1024 * 1024 * 80)
    g = p.open_graph(db, buffersize)

    home = pathlib.Path.home().resolve() / '.phist'
    session = PromptSession(history=FileHistory(str(home)))

    while True:
        try:
            qstr = session.prompt('poseidon> ', bottom_toolbar=bottom_toolbar, 
                                  multiline=True, prompt_continuation=prompt_continuation)
            res = g.query(qstr)

            for tup in res:
                print(tup)

        except KeyboardInterrupt:
            continue
        except EOFError:
            break
        except RuntimeError as e:
            print(f"ERROR: '{str(e)}'")
            continue

    print('GoodBye!')
#    res = g.query("Aggregate([count($4:qresult)], Match((w:Website)<-[:LINK]-(t:Tweet)-[:AUTHOR]->(a:Author)))")

    p.close()


def bottom_toolbar():
    return HTML('Poseidon Graph Database')

def prompt_continuation(width, line_number, is_soft_wrap):
    return '.' * width

if __name__ == "__main__":
    main()