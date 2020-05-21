CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -m32
PUBL=publications
DATA=data_structures
LIST=LinkedList
QUEUE=Queue
UTILS=utils

.PHONY: build clean

build: $(PUBL)_unlinked.o $(DATA)_unlinked.o $(LIST)_unlinked.o $(QUEUE)_unlinked.o $(UTILS)_unlinked.o
	ld -m elf_i386 -r $(PUBL)_unlinked.o $(DATA)_unlinked.o $(LIST)_unlinked.o $(QUEUE)_unlinked.o $(UTILS)_unlinked.o -o $(PUBL).o

$(PUBL)_unlinked.o: $(PUBL).c $(PUBL).h
	$(CC) $(CFLAGS) $(PUBL).c -c -o $(PUBL)_unlinked.o

$(DATA)_unlinked.o: $(DATA).c $(DATA).h
	$(CC) $(CFLAGS) $(DATA).c -c -o $(DATA)_unlinked.o

$(LIST)_unlinked.o: $(LIST).c $(LIST).h
	$(CC) $(CFLAGS) $(LIST).c -c -o $(LIST)_unlinked.o

$(QUEUE)_unlinked.o: $(QUEUE).c $(QUEUE).h
	$(CC) $(CFLAGS) $(QUEUE).c -c -o $(QUEUE)_unlinked.o

$(UTILS)_unlinked.o: $(UTILS).c $(UTILS).h
	$(CC) $(CFLAGS) $(UTILS).c -c -o $(UTILS)_unlinked.o

clean:
	rm -f *.o *.h.gch
