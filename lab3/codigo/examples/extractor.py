import subprocess
import os
import glob

def decompress_all_colb():
    colb_files = glob.glob('*.col.b')  
    for input_file in colb_files:
        output_file = input_file.replace('.b', '')
        try:
            subprocess.run(['./a.out', input_file, output_file], check=True) #cambiar a.out si se cambia el nombre del ejecutable o si se esta en windows
            print(f"Decompressed: {input_file} -> {output_file}")
def process_col_file(input_file, output_file=None):
    if output_file is None:
        output_file = input_file.replace('.col', '_edges.txt')
    with open(input_file, 'r') as f:
        lines = f.readlines()
        
    edges = []
    for line in lines:
        if line.startswith('e '):
            nums = line.strip().split()[1:]
            edges.append(' '.join(nums))
            
    with open(output_file, 'w') as f:
        f.write('\n'.join(edges))
if __name__ == "__main__":
    decompress_all_colb()
    for col_file in glob.glob('*.col'):
		process_col_file(col_file)
