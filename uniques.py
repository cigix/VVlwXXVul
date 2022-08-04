#!/usr/bin/env python3

'''Filter a list of words for five-letter words with 5 unique letters.'''

def isVlwVul(word):
    '''isVlwVul(word): Is five-letter word with 5 unique letters?

    Param:
      - word: str

    Return: boolean
    '''
    return len(word) == 5 and len(set(word)) == 5


def uniqueify(words):
    '''uniqueify(words): Make an generator of VlwVul.

    Param:
      - words: iterable of str

    Return: generator of str
    '''
    return filter(isVlwVul, words)


if __name__ == '__main__':
    import sys
    filename = sys.argv[1] if len(sys.argv) > 1 else "-"
    filehandle = open(filename) if filename != "-" else sys.stdin

    print('\n'.join(
        map(
            str.lower,
            uniqueify(
                map(
                    str.strip,
                    filehandle.readlines()
                )
            )
        )
    ))
