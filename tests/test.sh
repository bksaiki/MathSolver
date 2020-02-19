#!/bin/bash
failed=0
total=0
for file in $@
do
    "$file"
    failed+=$?
    total+=1
done

printf "%i/%i tests passed\n" $(expr $total - $failed) $total