#!/bin/bash
failed=0
total=0
for file in $@
do
    echo "Testing module:" $file
    eval $file
    if (( $? != 0 )); then
        echo "Module failed"
        failed+=1
    else
        echo "Module passed"
    fi
    total+=1
done

printf "%i/%i modules passed\n" $(expr $total - $failed) $total