#!/usr/bin/env python3

'''Group VlwVuls into VVlwXXVuls.'''

import itertools
import math
import multiprocessing
import tqdm

def getVVlwcode(Vlwcodes):
    '''getVVlwcode(Vlwcodes): Get the VVlwcode for Vlwcodes.

    Params:
      - Vlwcodes: tuple of 5 ints

    Return: int
    '''
    code1, code2, code3, code4, code5 = Vlwcodes
    return code1 | code2 | code3 | code4 | code5


XXVulcodes = {
    2 ** 26 - 1 - (2 ** i)
    for i in range(26)
}
def checkXXVulcode(Vlwcodes):
    VVlwcode = getVVlwcode(Vlwcodes)
    if VVlwcode in XXVulcodes:
        return Vlwcodes


def group(codes):
    '''group(codes): Find matching codes for VVlwXXVul.

    Params:
      - codes: iterable of int

    Return: set of frozensets of int
    '''

    ret = set()
    comblen = math.comb(len(codes), 5)

    with multiprocessing.Pool() as pool:
        VVlwXXulcodes = list(
            filter(
                None,
                tqdm.tqdm(
                    map(
                        checkXXVulcode,
                        itertools.combinations(codes, 5)),
                    total=comblen,
                    dynamic_ncols=True
                )
            )
        )

        #    if VVlwcode in XXVulcodes:
        #        ret.add(frozenset((code1, code2, code3, code4, code5)))

    return ret

if __name__ == '__main__':
    import sys
    filename = sys.argv[1] if len(sys.argv) > 1 else "-"
    filehandle = open(filename) if filename != "-" else sys.stdin

    codes = list()
    for line in map(str.strip, filehandle.readlines()):
        codestr, _ = line.split(',', maxsplit=1)
        codeint = int(codestr, 16)
        codes.append(codeint)

    group(codes)
