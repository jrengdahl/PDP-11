import re
import sys

def convert_hex_to_oct(match):
    hex_value = match.group(0)
    oct_value = format(int(hex_value, 16), '06o')
    return oct_value

def convert_disassembly_line(line):
    # Extract address
    parts = line.split('\t', 1)
    address = parts[0][:-1]  # Remove the colon
    rest_of_line = parts[1] if len(parts) > 1 else ''
    address_oct = format(int(address, 16), '06o') + ':'

    # Split the rest of the line to handle opcodes and disassembly text
    match = re.match(r'(\s*)([0-9a-fA-F]+)( [0-9a-fA-F]+)?( [0-9a-fA-F]+)?(.*)', rest_of_line)
    if not match:
        return line  # return the original line if it doesn't match the expected format

    spaces, opcode1, opcode2, opcode3, disasm_text = match.groups()

    # Convert opcode fields to octal
    opcode1 = format(int(opcode1, 16), '06o')
    opcode2 = format(int(opcode2.strip(), 16), '06o') if opcode2 else ''
    opcode3 = format(int(opcode3.strip(), 16), '06o') if opcode3 else ''

    # Convert hex numbers in disassembly text
    disasm_text = re.sub(r'0x[0-9a-fA-F]+', convert_hex_to_oct, disasm_text)
    
    return f"{address_oct}\t{opcode1} {opcode2} {opcode3} {disasm_text}".rstrip()

def process_line(line):
    line = line.rstrip()  # Remove trailing whitespace
    if re.match(r'^\s*[0-9a-fA-F]+:\t', line):
        # Disassembly line
        return convert_disassembly_line(line)
    elif re.match(r'^[0-9a-fA-F]+\s', line):
        # Label line
        parts = line.split(None, 1)
        label = parts[0]
        rest_of_line = parts[1] if len(parts) > 1 else ''
        label_oct = format(int(label, 16), '06o')
        return f"{label_oct} {rest_of_line}"
    else:
        # Other lines
        return line

if __name__ == "__main__":
    if len(sys.argv) > 2:
        print("Usage: python3 hex2oct.py [input_file]")
        sys.exit(1)

    if len(sys.argv) == 2:
        input_source = open(sys.argv[1], 'r')
    else:
        input_source = sys.stdin

    with input_source as infile:
        for line in infile:
            print(process_line(line))

