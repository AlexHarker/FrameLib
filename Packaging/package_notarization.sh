#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

PACKAGE_PATH="$DIR/Max/FrameLib"
EXTERNAL_PATH="$PACKAGE_PATH/externals"
NOTARIZE="$DIR/package_notarize_external.sh"

RESULT=0
COMPLETED=0
pids=""

cd "$EXTERNAL_PATH"

LIST=`find *.mxo -depth 0`

# Notarize externals

notarise()
{
    "$NOTARIZE" "$EXTERNAL_PATH" "$1"
}

for filename in $LIST
do
   notarise "$filename" &
   pids+="$! "
done

for pid in $pids; do
    wait $pid
    if [ $? -eq 0 ]; then
        let COMPLETED+=1
    else
        RESULT=1
        echo "FAILED - Job $pid exited with a status of $?"
    fi
done

if [ $RESULT -eq 0 ]; then
    echo "SUCCESS - All Externals Notarized! $COMPLETED Completed"
else
    echo "FAILED - Some Externals Failed to Notarize! $COMPLETED Completed"
fi

