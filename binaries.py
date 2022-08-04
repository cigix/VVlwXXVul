#!/usr/bin/env python3

'''Turns the codes output by codes.py into pure binary form.'''

if __name__ == '__main__':
    import sys
    filename = sys.argv[1] if len(sys.argv) > 1 else "-"
    filehandle = open(filename) if filename != "-" else sys.stdin

    for line in map(str.strip, filehandle.readlines()):
        codestr, _ = line.split(',', maxsplit=1)
        codeint = int(codestr, 16)
        codebytes = codeint.to_bytes(4, sys.byteorder)
        sys.stdout.buffer.write(codebytes)
