#!/bin/bash

if [ $# -eq 0 ]; then
echo "Usage: dump_strings <SRC_DIR> [HASH_PROGRAM]"
exit 1
fi

SRC=$1

if [ $# -lt 2 ]; then
HASH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )/hash"
else
HASH=$2
fi

if [ ! -d $SRC ]; then
echo "$SRC not found"
exit 1
fi

if [ ! -f $HASH ]; then
echo "$HASH not found"
exit 1
fi

mapfile -t STRINGS < <(grep -roh 'LOCALIZE\(_STATIC\)\?("[^"]\+")' $SRC | cut -d \" -f 2 | sort | uniq)
echo num_strings: ${#STRINGS[@]}
for STRING in "${STRINGS[@]}"
do
	echo id: $($HASH $STRING)
	echo str: $(cat /dev/urandom | tr -dc 'a-z ' | fold -w ${#STRING} | head -n 1)
done
