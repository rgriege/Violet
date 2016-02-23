#!/bin/sh

additions=("block" "entity" "view")
exceptions=("v2" "v2i" "v3" "m2" "m3" "m4" "main" "aabb" "vector" "memory")

files=$(find . -type f)
#concat additions and files

for line in $files; do
	IFS='/'
	pieces=( $line )
	file="${pieces[-1]}"
	name="${file%.*}"
	if [[ "${exceptions[@]}" =~ "$name" ]]; then
		echo "skipping $name"
	else
			IFS='_'
			words=( $name )
			words=( "${words[*]^}" )
			rename="${words[*]}"
			echo "replacing $name with $rename"
			echo "$files" | xargs sed -i "s/\b$name\b/$rename/"
	fi
done 

