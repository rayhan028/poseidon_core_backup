import csv
import os
import sys

# --- Constants ---
DELIMITER = '|'
STATIC_START_TIME = 946684800000 
INFINITY = 253402300799000 

def process_file(in_path, out_path, is_static=False):
    """
    Simply adds ValidFrom and ValidTo columns. 
    """
    with open(in_path, 'r', encoding='utf-8') as fin:
        reader = csv.DictReader(fin, delimiter=DELIMITER)
        fieldnames = reader.fieldnames + ['ValidFrom', 'ValidTo']
        
        with open(out_path, 'w', encoding='utf-8') as fout:
            writer = csv.DictWriter(fout, fieldnames=fieldnames, delimiter=DELIMITER)
            writer.writeheader()
            
            for row in reader:
                creation_val = row.get('creationDate')
                
                # Use original creationDate as ValidFrom. 
                # If static, use the 2000-01-01 timestamp.
                row['ValidFrom'] = int(creation_val) if creation_val and not is_static else STATIC_START_TIME
                
                # All data starts as 'current' (ends at Infinity)
                row['ValidTo'] = INFINITY
                writer.writerow(row)

def run_enhancement(sf_name):
    in_base = os.path.join("Temporal-data-LDBC", sf_name)
    out_base = os.path.join("Temporal-data-LDBC", f"{sf_name}_enhanced")
    
    if not os.path.exists(in_base):
        print(f"Error: {in_base} not found.")
        return

    for category in ['static', 'dynamic']:
        in_dir = os.path.join(in_base, category)
        out_dir = os.path.join(out_base, category)
        
        if not os.path.exists(in_dir):
            continue
            
        os.makedirs(out_dir, exist_ok=True)
        
        for filename in os.listdir(in_dir):
            if filename.endswith(".csv"):
                print(f"Enhancing: {category}/{filename}")
                process_file(os.path.join(in_dir, filename), os.path.join(out_dir, filename), category == 'static')

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 temporalize_ldbc.py sf1")
    else:
        run_enhancement(sys.argv[1])