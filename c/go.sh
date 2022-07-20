#!/usr/bin/env bash

DIR=$(cd $( dirname ${BASH_SOURCE[0]} ) && pwd)

cd "$DIR"

# Watch for changes if the user typed "./go.sh watch"
if [ "$1" == 'watch' ] ; then
    nodemon
fi

# build everything, exit on any error
set -e
clang test.c -o test
clang atomic-test.c -o atomic-test -pthread

BOLD_RED='\x1B[1;31m'
BOLD_YELLOW='\x1B[1;33m'
BOLD_CYAN='\x1B[1;36m'
BOLD_WHITE='\x1B[1;1m'
BOLD_GREEN='\x1B[1;32m'
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

            fail='definitely failed'
        fi
    done

    if [ -n "$fail" ]; then return 1 ; fi
}

# run tests, allow them to fail assertions
set +e
for test in test atomic-test ; do
    if [ -e $test ]; then
        printf "${BOLD_CYAN}\nExecuting \"./$test\"\n${RESET}"
        "./$test" 3>&1 1>&2- 2>&3- | process
        if [ $? -eq 0 ]; then
            printf "$BOLD_GREEN[ok]${RESET} \"./$test\" passed\n" 
        else
            printf "$BOLD_RED[not ok]${RESET} \"./$test\" failed\n"
        fi
    else
        printf "${BOLD_RED}Executable not found: ${BOLD_WHITE}$test${RESET}"
    fi
done
echo