#!/usr/bin/env python3

"""
expand_template
-----------

Lluvia template expansion command line tool
"""

import argparse
import os
import sys
import jinja2


SEPARATOR = ':'

def parseStrings(arr, symbols):
    
    for sym in arr:
        split = sym.split(SEPARATOR, maxsplit=1)

        if len(split) != 2:
            print('Error parsing symbol definition "{0}". The symbol name and value must be separated by "{1}"'.format(sym, SEPARATOR), file=sys.stderr)
            sys.exit(1)
        
        symbols[split[0]] = split[1]


def parseFiles(arr, symbols):
    
    for sym in arr:
        split = sym.split(SEPARATOR, maxsplit=1)

        if len(split) != 2:
            print('Error parsing symbol definition "{0}". The symbol name and value must be separated by "{1}"'.format(sym, SEPARATOR), file=sys.stderr)
            sys.exit(1)
        
        try:
            with open(split[1]) as f:
                value = f.read()
                symbols[split[0]] = value

        except IOError as e:
            print('Error reading file: {0}'.format(e), file=sys.stderr)
            sys.exit(1)


def main():

    parser = argparse.ArgumentParser()
    parser.add_argument('-o', '--output', type=str, help='output file path.')
    parser.add_argument('-D', type=str, metavar='D', action='append', help='defines a string symbol.')
    parser.add_argument('-F', type=str, metavar='F', action='append', help='defines a file symbol.')
    parser.add_argument('input', type=str, help='input template file.')
    
    args = parser.parse_args()

    symbols = dict()
    if args.D != None:
        parseStrings(args.D, symbols)
    
    if args.F != None:
        parseFiles(args.F, symbols)

    try:
        with open(args.input) as f:
            template = jinja2.Template(f.read())

            out = template.render(**symbols)
            
            outPath = args.output if args.output != None else args.input + ".txt"

            try:
                with open(outPath, mode='w') as o:
                    print(out)
                    o.write(out)
            except IOError as eo:
                print('Error creating output file "{0}": {1}'.format(outPath, eo))
                sys.exit(1)

    except IOError as e:
        print('Error reading template file "{0}": {1}'.format(args.input, e), file=sys.stderr)
        sys.exit(1)

    sys.exit(0)


if __name__ == "__main__":
    main()
