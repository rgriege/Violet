#!/bin/bash

if [ $# -eq 0 ]; then
echo "Usage: dump_strings <SRC_DIR> [HASH_PROGRAM] [FIND_PROGRAM]"
exit 1
fi

SRC=$1

if [ $# -lt 2 ]; then
HASH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )/hash"
else
HASH=$2
fi

if [ $# -lt 3 ]; then
FIND="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )/localize_find"
else
FIND=$3
fi

if [ ! -d $SRC ]; then
echo "$SRC not found"
exit 1
fi

if [ ! -f $HASH ]; then
echo "$HASH not found"
exit 1
fi

if [ ! -f $FIND ]; then
echo "$FIND not found"
exit 1
fi

mapfile -t STRINGS < <(find $SRC -type f | xargs $FIND | sort | uniq)
echo num_strings: ${#STRINGS[@]}
for STRING in "${STRINGS[@]}"
do
	echo id: $($HASH $STRING)
	echo str: $STRING
done
