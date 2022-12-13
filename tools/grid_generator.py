import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--grid-size", help="Number of cells in one grid row", type=int, required=True)
parser.add_argument("--file-name", help="Output file name", type=str, required=True)
args = parser.parse_args()

with open(args.file_name, "w") as network_file:
	for x in range(args.grid_size):
		for y in range(args.grid_size):
			node_id = x + args.grid_size*y + 1
			left_neighbour = node_id-1 if x != 0 else None
			right_neighbour = node_id + 1 if x != args.grid_size-1 else None
			up_neighbour = node_id + args.grid_size if y != args.grid_size-1 else None
			down_neighbour = node_id - args.grid_size if y != 0 else None
			
			record = f"{node_id}:"
			for neighbour in [left_neighbour, right_neighbour, up_neighbour, down_neighbour]:
				if neighbour is not None:
					if record[-1].isdigit():
						record += ","
					record += str(neighbour)
			
			record += '\n'
			network_file.write(record)
