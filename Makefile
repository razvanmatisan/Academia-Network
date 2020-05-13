CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -m32
PUBL=publications
DATA=data_structures

.PHONY: build clean

build: $(PUBL)_unlinked.o $(DATA)_unlinked.o
	ld -m elf_i386 -r $(PUBL)_unlinked.o $(DATA)_unlinked.o -o $(PUBL).o

$(PUBL)_unlinked.o: $(PUBL).c $(PUBL).h
	$(CC) $(CFLAGS) $(PUBL).c -c -o $(PUBL)_unlinked.o

$(DATA)_unlinked.o: $(DATA).c $(DATA).h
	$(CC) $(CFLAGS) $(DATA).c -c -o $(DATA)_unlinked.o

clean:
	rm -f *.o *.h.gch
