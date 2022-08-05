# VVlwXXVul

## What's with this weird name?

Unimportant words are ignored, important words are initialised, and numbers are
written in capital roman numerals, hence VVlwXXVul: 5 5-letter words (with) 25
unique letters.

There are places in the code where this naming convention is used, you should
get familiar with it now.

## What is this for?

This set of programs solves the problem set in [this Matt Parker video: "Can you find: five five-letter words with twenty-five unique letters?"](https://youtu.be/_-AfhLQfb6w).

The solution presented here was intended to be novel when compared to the two
solutions presented in the video. It also happened to be faster, albeit less
portable.

## The novelty

We are trying to solve the problem of considering the letters of 5 different
5-letter words to find 25 unique letters. This is inherently a set theory
problem: given sets of letters, find 5 sets whose union has cardinality 25.

The first trick applied is to turn set manipulations into binary operations:
since there are 26 distinct letters in the alphabet commonly used in English,
the set of letters of any English word can be expressed as a sequence of 26
bits. We then have the following operations:

* union of sets => pairwise OR of the sequences
* cardinality of set => counting the bits that are set, a.k.a. bit count, a.k.a.
  popcount ("population count")

Another advantage of using sequences of bits instead of words is that anagrams
are easily handled: they turn into the same sequence.

This trick on its own reduced the computation time in Python from the 31 days on
Matt's laptop, to a projected 15 days on my computer. But we can go faster.

The second trick is to exploit the fact that computers are _really good_ with
binary numbers. By rewriting all the combinatorics in C, I was able to make sure
that the essential set operations were properly optimised into a minimal set of
processor instructions.

The third trick is parallelisation. I'm using processes because they are easier
to handle in C.

As it stands, this implementation is able to work its way through the original
370105 words list in less than 30 seconds.

## What am I looking at?

This implementation has 5 steps.

1. `uniques.py` turns any list of words into a list of 5-letter words with no
   duplicate letters.
2. `codes.py` associates every word in a list, to a binary code corresponding to
   the position of its letters, expressed as 32-bit hexadecimal. Since anagrams
   have the same code, the result is rather expressed as a map from code to list
   of anagrams.
3. `binaries.py` takes the list of codes written as text by `codes.py` and
   rewrites it in binary. This allows the matching to be done on the codes only,
   without considering the words, and with much easier parsing.
4. `match.c` runs through all 5-code combinations and outputs the ones that have
   popcount (=cardinality) 25.
5. `resolve.py` matches back the code combinations output by `match.c` with the
   words they came from (see step 2) and into a usable JSON format.

## How do I run this?

You will need:

* a POSIX environment
* Python 3.6 or later (f-strings)
* a C compiler
* an x86 CPU (`popcnt` instruction)
* `make` (e.g. from GNU)
* `tee` (e.g. from GNU coreutils)
* a list of words. See description of the video.

You may need:

* GCC. I don't know how deep the intrinsics call I use depend on the compiler
  and architecture.

Place the list of words in a file called `words.txt` and run `make`. You may
also override the default list and or the number of processes with `make
WORDLIST=<path to file> PROCESS_COUNT=<count>`. You may also run every step
manually by feeding in the files and piping the output appropriately; be careful
with the compilation of `match.c`.

The results are put in `results.json`.
