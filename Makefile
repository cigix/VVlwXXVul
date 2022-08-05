WORDLIST?=words.txt
PROCESS_COUNT?=8

CFLAGS=-O3 -g3 -march=native -DMAX_PROCESS_COUNT=$(PROCESS_COUNT)

all: results.json

uniques.txt: uniques.py $(WORDLIST)
	./uniques.py $(WORDLIST) > $@

codes.txt: codes.py uniques.txt
	./codes.py uniques.txt > $@

binaries.dat: binaries.py codes.txt
	./binaries.py codes.txt > $@

matches.txt: match binaries.dat
	./match binaries.dat | tee $@

results.json: resolve.py codes.txt matches.txt
	./resolve.py codes.txt matches.txt > $@

clean:
	$(RM) uniques.txt codes.txt binaries.dat match matches.txt results.json
