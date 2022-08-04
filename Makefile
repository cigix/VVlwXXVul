WORDLIST?=words.txt

CFLAGS=-O3 -g3 -march=native

all: matches.txt

uniques.txt: uniques.py $(WORDLIST)
	./uniques.py $(WORDLIST) > $@

codes.txt: codes.py uniques.txt
	./codes.py uniques.txt > $@

binaries.dat: binaries.py codes.txt
	./binaries.py codes.txt > $@

matches.txt: match binaries.dat
	./match binaries.dat | tee $@

clean:
	$(RM) uniques.txt codes.txt binaries.dat match matches.txt
