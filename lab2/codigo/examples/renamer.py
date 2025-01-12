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
for col_file in glob.glob('*.col'):
    process_col_file(col_file)
