#!/bin/bash

# Zips all relevant files and exports those needed to testing zone
ARCHIVE=SD_T3
PUBL=publications
HT=Hashtables
LIST=LinkedList
Q=Queue
UTILS=utils
MAKE=Makefile
EXPORT=../AN_Checking # Replace with your testing zone

# Cleaning environment
make clean
make -f Makefile_tema3 clean

# Zipping
rm $ARCHIVE.zip
zip $ARCHIVE.zip $PUBL.* $HT.* $LIST.* $Q.* $UTILS.* $MAKE

# Exporting
unzip $ARCHIVE.zip -d $EXPORT
