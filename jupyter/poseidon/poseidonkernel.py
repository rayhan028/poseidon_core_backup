from ipykernel.kernelbase import Kernel
from tabulate import tabulate
import logging
import sys
import pathlib
import os

lib_path = pathlib.Path(__file__).parent.parent.parent.resolve() / 'build'
if os.path.isdir(lib_path):
    sys.path.append(str(lib_path))
else:
    print(f"Cannot infer lib_path {lib_path}")

import poseidon

class PoseidonKernel(Kernel):
    implementation = 'Poseidon'
    implementation_version = '1.0'
    language = 'poseidon'
    language_version = '0.1'
    language_info = {'name': 'poseidon', 
                     'mimetype': 'application/poseidon-ql',
                     'file_extension': '.pql'}
    banner = "Poseidon kernel - access to Poseidon Graph Database"

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        logging.basicConfig(filename="kernel.log", level=logging.INFO) 
        self.pool = None
        self.db = None

    def print_data(self, *data: str, mime: str = 'text/html'):
        for v in data:
            self.send_response(self.iopub_socket, 'display_data', {
                'data': {
                    mime: v
                },
                # `metadata` is required. Otherwise, Jupyter Lab does not display any output.
                # This is not the case when using Jupyter Notebook btw.
                'metadata': {}
            })

    def handle_cmd(self, code):
        cmds = code.split(" ")
        if cmds[0].upper() == "%OPEN" and cmds[2].upper() == "IN":
            pool_name = cmds[3].strip("'")
            db_name = cmds[1].strip("'")
            logging.info("TRYING TO OPEN: %s/%s", pool_name, db_name)
            try:
                self.pool = poseidon.open_pool(pool_name, 1024 * 1024 * 80)
                self.db = self.pool.open_graph(db_name, 10000)
                return "Poseidon database opened successfully."
            except RuntimeError as e:
                return str(e)
        elif cmds[0] == "%CREATE".upper() and cmds[2].upper() == "IN":
            pool_name = cmds[3].strip("'")
            db_name = cmds[1].strip("'")
            logging.info("TRYING TO CREATE: %s/%s", pool_name, db_name)
            try:
                self.pool = poseidon.create_pool(pool_name, 1024 * 1024 * 80)
                self.db = self.pool.create_graph(db_name, 10000)
                return "Poseidon database created successfully."
            except RuntimeError as e:
                return str(e)
        elif cmds[0].upper() == "%CLOSE":
            self.pool.close()
            return "Poseidon database closed."
        else:
            logging.info("UNKNOWN CMD: '%s'", code)
            return f"Ooops: unknown command {code}"


    def rows_table(self, rows) -> str:
        return ''.join(map(
            lambda row: '<tr>' + ''.join(map(lambda e: f'<td style="text-align: left">{e}</td>', row)) + '</tr>',
            rows
        ))
    
    def handle_query(self, code):
        logging.info("EXEC QUERY: '%s'", code)
        res = self.db.query(code)

        table_data = self.rows_table(res)
 
        return f'''
                <table class="poseidon-query-result">
                    {table_data}
                </table>
            '''
    
    def do_execute(self, code, silent, store_history=True, user_expressions=None,
                   allow_stdin=False):
        
        logging.info("code='%s'", code)

        res = ""
        if code.startswith('%'):      
            res = self.handle_cmd(code)
            if not silent:
                stream_content = {'name': 'stdout', 'text': res}
                self.send_response(self.iopub_socket, 'stream', stream_content)
        else:
            res = self.handle_query(code)
            self.print_data(res)

        return {'status': 'ok',
                # The base class increments the execution count
                'execution_count': self.execution_count,
                'payload': [],
                'user_expressions': {},
               }

if __name__ == '__main__':
	from ipykernel.kernelapp import IPKernelApp
	IPKernelApp.launch_instance(kernel_class=PoseidonKernel)
