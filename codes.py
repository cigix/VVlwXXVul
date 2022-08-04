#!/usr/bin/env python3

'''Associate a code to each word. Codes are shared between anagrams.'''

def getoffset(letter):
    '''getoffset(letter): Get the code identifying the letter

    Params:
      - letter: str of length 1, in [a-z]

    Return: int
    '''
    return 2 ** (ord(letter) - ord('a'))


def getcode(word):
    '''getcode(word): Get a code identifying the letters in word.

    Params:
      - word: str

    Return: int
    '''
    return sum(map(getoffset, word))


def getcodes(words):
    '''getcodes(words): Get the codes identifying words.

    Params:
      - words: iterable of str

    Return: dict of int to set of str
    '''
    codes = dict()
    for word in words:
        codes.setdefault(getcode(word), set()).add(word)
    return codes


if __name__ == '__main__':
    import sys
    filename = sys.argv[1] if len(sys.argv) > 1 else "-"
    filehandle = open(filename) if filename != "-" else sys.stdin

    codes = getcodes(map(str.strip, filehandle.readlines()))
    for code, words in codes.items():
        print(f"{code:#010x},{','.join(sorted(words))}")
