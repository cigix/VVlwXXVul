#!/usr/bin/env python3

if __name__ == '__main__':
    import json
    import sys
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv} <codes> [matches]")
        sys.exit(1)

    codes = dict() # str to list of str
    with open(sys.argv[1]) as filehandle:
        for line in filehandle.readlines():
            codestr, anagrams = line.strip().split(',', maxsplit=1)
            codes[codestr] = anagrams.split(',')

    filename = sys.argv[2] if len(sys.argv) > 2 else "-"
    filehandle = open(filename) if filename != "-" else sys.stdin

    matches = list() # list of list of str
    for line in filehandle.readlines():
        matches.append(line.strip().split(','))

    results = list()
    for matchcodes in matches:
        matchwords = list(map(codes.get, matchcodes))
        XXVulcode = sum(map(lambda s: int(s, 16), matchcodes))
        notXXVulcode = 2 ** 26 - 1 - XXVulcode
        missingpos = notXXVulcode.bit_length() - 1
        missing = chr(missingpos + ord('a'))
        results.append({
            "words": matchwords,
            "missing": missing
        })

    json.dump(results, sys.stdout)
