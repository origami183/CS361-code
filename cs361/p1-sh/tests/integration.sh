#!/bin/bash

EXE="../dukesh"

function run_test {

    # parameters
    TAG=$1
    ARGS=$2

    # file paths
    OUTPUT=outputs/$TAG.txt
    DIFF=outputs/$TAG.diff
    EXPECT=expected/$TAG.txt
    VALGRND=valgrind/$TAG.txt

    # run test and compare output to the expected version
    $EXE $ARGS 2>/dev/null >"$OUTPUT"
    diff -u "$OUTPUT" "$EXPECT" >"$DIFF"
    PTAG=$(printf '%-30s' "$TAG")
    if [ -s "$DIFF" ]; then

        v=2
        while [[ $v < 5 ]] ; do
            EXPECT=expected/$TAG-$v.txt
            if [ -e "$EXPECT" ]; then
                diff -u "$OUTPUT" "$EXPECT" >"$DIFF"
                if [ ! -s "$DIFF" ]; then
                    echo "$PTAG pass"
                    v=6
                fi
            fi
            v=$((v+1))
        done
        if [[ $v < 6 ]] ; then
            echo "$PTAG FAIL (see $DIFF for details)"
        fi
    else
        echo "$PTAG pass"
    fi

    # run valgrind
    valgrind $EXE $ARGS &>$VALGRND
}

# initialize output folders
mkdir -p outputs
mkdir -p valgrind
rm -f outputs/* valgrind/*

# run individual tests
source itests.include

# check for memory leaks
LEAK=`cat valgrind/*.txt | grep 'definitely lost' | grep -v ' 0 bytes in 0 blocks'`
if [ -z "$LEAK" ]; then
    echo "No memory leak found."
else
    echo "Memory leak(s) found. See files listed below for details."
    grep 'definitely lost' valgrind/*.txt | grep -v ' 0 bytes in 0 blocks' | sed -e 's/:.*$//g' | sed -e 's/^/  - /g'
fi

