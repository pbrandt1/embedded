#!/usr/bin/env bash

set -e

DIR=$(cd $( dirname ${BASH_SOURCE[0]} ) && pwd)

cd "$DIR"

# Watch for changes if the user typed "./go.sh watch"
if [ "$1" == 'watch' ] ; then
    nodemon
fi

# build
gfortran test.f90 -o test -std=f2008 -Wall

BOLD_RED='\x1B[1;31m'
BOLD_YELLOW='\x1B[1;33m'
BOLD_WHITE='\x1B[1;1m'
RESET='\x1B[0m'

function process() {
    while read line
    do
        if [[ "${line}" =~ "failed" ]] ; then
            stuff=$(echo $line | sed 's/.* \(.*\.c\):\([0-9]*\).*/\1,\2/')
            file=$(echo $stuff | sed 's/\(.*\),.*/\1/' )
            line=$(echo $stuff | sed 's/.*,\(.*\)/\1/' )
            printf "${BOLD_WHITE}$file:$line: ${BOLD_YELLOW}failed assertion${RESET}\n${BOLD_WHITE}"
            sed -n ${line}p $file
            printf "${RESET}"
        fi
    done
}

# run
# ./test 3>&1 1>&2- 2>&3- | sed "s/\(Assertion.*failed\)/${BOLD_RED}\1${RESET}/"
./test 3>&1 1>&2- 2>&3- | process
