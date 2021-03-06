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
#   
#  'decrement'     decreases the version number, see 'increment' for methods
#
#  'set'           sets the version number
#
#  'drop'          drops any version tag: '1.0.0-dev' ==> '1.0.0' 
#
#  'tag'           adds a version tag: '1.0.0' ==> '1.0.0-tag' 
#

ver_file="lib/common/base.h"
full_version=$(echo $(grep -oP '(?<=MATHSOLVER_VERSION_STR\s)[^ ]*' $ver_file) | sed 's/"//g')
version=$(echo $full_version | sed -nre 's/^(([0-9]+\.)*[0-9]+).*/\1/p')
ver_comp=($(echo ${version} | sed -e 's/\./\n/g' | sed -e '/^[0-9]*$/!d'))
new_version=""

if [ $1 == "current" ]; then
    echo $full_version
    exit 0
elif [[ $1 == "increment" ]]; then
    if [[ $2 == "patch" ]]; then
        new_version="${ver_comp[0]}.${ver_comp[1]}.$((1+${ver_comp[2]}))"
    elif [[ $2 == "minor" ]]; then
        new_version="${ver_comp[0]}.$((1+${ver_comp[1]})).${ver_comp[2]}"
    elif [[ $2 == "major" ]]; then
        new_version="$((1+${ver_comp[0]})).${ver_comp[1]}.${ver_comp[2]}"
    else 
        exit 1
    fi
elif [[ $1 == "decrement" ]]; then
    if [[ $2 == "patch" ]] && ! [ ${ver_comp[2]} -eq 0 ]; then
        new_version="${ver_comp[0]}.${ver_comp[1]}.$((${ver_comp[2]}-1))"
    elif [[ $2 == "minor" ]] && ! [ ${ver_comp[1]} -eq 0 ]; then
        new_version="${ver_comp[0]}.$((${ver_comp[1]}-1)).${ver_comp[2]}"
    elif [[ $2 == "major" ]] && ! [ ${ver_comp[0]} -eq 0 ]; then
        new_version="$((${ver_comp[0]}-1)).${ver_comp[1]}.${ver_comp[2]}"
    else 
        exit 1
    fi
elif [[ $1 == "set" ]]; then
    if [[ $2 =~ ^([0-9]+)\.([0-9]+)\.([0-9]+)(-([a-zA-Z0-9]+))*$ ]]; then
        new_version=$2
    else
        echo "Invalid version"
        exit 1
    fi
elif [[ $1 == "drop" ]]; then
    new_version=$version
elif [[ $1 == "tag" ]]; then
    new_version=$version-$2
else
    exit 1
fi

# sets new version
echo "Changing version from $full_version to $new_version"
new_ver=$(echo $new_version | sed -nre 's/^(([0-9]+\.)*[0-9]+).*/\1/p')
new_ver_comp=($(echo ${new_ver} | sed -e 's/\./\n/g' | sed -e '/^[0-9]*$/!d'))
sed -i "s/$full_version/$new_version/g" $ver_file
sed -i "s/MATHSOLVER_VERSION_MAJOR\([[:space:]]\+\)${ver_comp[0]}/MATHSOLVER_VERSION_MAJOR\1${new_ver_comp[0]}/g" $ver_file
sed -i "s/MATHSOLVER_VERSION_MINOR\([[:space:]]\+\)${ver_comp[1]}/MATHSOLVER_VERSION_MINOR\1${new_ver_comp[1]}/g" $ver_file
sed -i "s/MATHSOLVER_VERSION_PATCH\([[:space:]]\+\)${ver_comp[2]}/MATHSOLVER_VERSION_PATCH\1${new_ver_comp[2]}/g" $ver_file
exit 0