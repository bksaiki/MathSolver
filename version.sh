#!/bin/bash

#  Version management
#   
#  Commands:
#
#  'current'       prints the current version of the repo
#
#  'increment'     increases the version number by:
#    'major'       1.0.0 -> 2.0.0
#    'minor'       1.0.0 -> 1.1.0
#    'patch'       1.0.0 -> 1.0.1
#    'revision'    1.0.0 -> 1.0.0-rev1
#                  1.0.0-rev1 -> 1.0.0-rev2
#   
#  'decrement'     decreases the version number, see 'increment' for methods
#
#  'set'           sets the version number

ver_file="lib/common/base.h"
rev_str="rev"
full_version=$(echo $(grep -oP '(?<=MATHSOLVER_VERSION_STR\s)[^ ]*' $ver_file) | sed 's/"//g')
version=${full_version%-$rev_str*}
ver_comp=($(echo ${version} | sed -e 's/\./\n/g' | sed -e '/^[0-9]*$/!d'))
new_version=""

if [ $1 == "current" ]; then
    echo $full_version
    exit 0
elif [[ $1 == "increment" ]]; then
    if [[ $2 == "revision" ]]; then
        if [[ $full_version == *$rev_str* ]]; then
            new_version=$version-$rev_str$((1+$(echo $full_version | grep -Eo '[0-9]+$')))
        else
            new_version="$full_version-$rev_str"1
        fi
    elif [[ $2 == "patch" ]]; then
        new_version="${ver_comp[0]}.${ver_comp[1]}.$((1+${ver_comp[2]}))"
    elif [[ $2 == "minor" ]]; then
        new_version="${ver_comp[0]}.$((1+${ver_comp[1]})).${ver_comp[2]}"
    elif [[ $2 == "major" ]]; then
        new_version="$((1+${ver_comp[0]})).${ver_comp[1]}.${ver_comp[2]}"
    else 
        exit 1
    fi
elif [[ $1 == "decrement" ]]; then
    if [[ $2 == "revision" ]]; then
        if [[ $full_version == *$rev_str* ]]; then
            new_version=$version-$rev_str$((1+$(echo $full_version | grep -Eo '[0-9]+$')))
        else
            new_version="$full_version-$rev_str"1
        fi
    elif [[ $2 == "patch" ]] && ! [ ${ver_comp[2]} -eq 1 ]; then
        new_version="${ver_comp[0]}.${ver_comp[1]}.$((${ver_comp[2]}-1))"
    elif [[ $2 == "minor" ]] && ! [ ${ver_comp[1]} -eq 1 ]; then
        new_version="${ver_comp[0]}.$((${ver_comp[1]}-1)).${ver_comp[2]}"
    elif [[ $2 == "major" ]] && ! [ ${ver_comp[0]} -eq 1 ]; then
        new_version="$((${ver_comp[0]}-1)).${ver_comp[1]}.${ver_comp[2]}"
    else 
        exit 1
    fi
elif [[ $1 == set ]]; then
    if [[ $2 =~ ^([0-9]+)\.([0-9]+)\.([0-9]+)(-${rev_str}([0-9]+))*$ ]]; then
        new_version=$2
    else
        echo "Invalid version"
        exit 1
    fi
else
    exit 1
fi

# sets new version
echo "Changing version from $full_version to $new_version"
sed -i "s/$full_version/$new_version/g" $ver_file
exit 0