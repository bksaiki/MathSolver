#!/bin/bash
failed=0
total=0
for file in $@
do
    eval $file
    if (( $? != 0 )); then
        failed+=1
    fi
    total+=1
done

printf "%i/%i tests passed\n" $(expr $total - $failed) $total