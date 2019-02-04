#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

mapfile -t STRINGS < <(grep -roh 'LOCALIZE\(_STATIC\)\?("[^"]\+")' $1 | cut -d \" -f 2 | sort | uniq)
echo num_strings: ${#STRINGS[@]}
for STRING in "${STRINGS[@]}"
do
	echo id: $($DIR/hash $STRING)
	echo str: $STRING
done
