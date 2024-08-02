import re
import sys

def convert_hex_to_oct(match):
    hex_value = match.group(0)
    oct_value = format(int(hex_value, 16), 'o')
    return oct_value

def convert_line_to_octal(line):
    # Split the line into components
    parts = line.split()
    
    # Convert address to octal
    if len(parts) > 0 and re.match(r'^[0-9a-fA-F]+:$', parts[0]):
        address = parts[0][:-1]
        parts[0] = f"{int(address, 16):07o}:"

    # Convert opcode fields to octal
    i = 1
    opcodes = []
    while i < len(parts) and re.match(r'^[0-9a-fA-F]+$', parts[i]):
        opcodes.append(f"{int(parts[i], 16):06o}")
        i += 1

    # Join the opcodes with spaces
    opcode_str = ' '.join(opcodes)
    
    # Calculate the padding based on the number of opcode words
    padding = ' ' * (20 - len(opcode_str))

    # Rejoin the parts and convert any hex numbers in comments
    remaining_line = ' '.join(parts[i:])
    remaining_line = re.sub(r'0x[0-9a-fA-F]+', convert_hex_to_oct, remaining_line)
    
    # Construct the final line with aligned columns
    final_line = f"{parts[0]} {opcode_str}{padding}{remaining_line}"
    return final_line

if __name__ == "__main__":
    if len(sys.argv) > 2:
        print("Usage: python3 convert_hex_to_oct.py [input_file]")
        sys.exit(1)

    if len(sys.argv) == 2:
        input_source = open(sys.argv[1], 'r')
    else:
        input_source = sys.stdin

    # Read and process the input
    with input_source as infile:
        for line in infile:
            if line.strip():
                print(convert_line_to_octal(line))
            else:
                print('')

