#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Variables

PACKAGE_PATH="$DIR/FrameLib"
EXTERNAL_PATH="$PACKAGE_PATH/externals"
NOTARIZE_PATH="$EXTERNAL_PATH/FrameLib_Notarisation"
TIME=`date +"%d-%m-%y-%T"`
TIMESTAMP="${TIME//:/}"
ZIP_PATH="$EXTERNAL_PATH/FrameLib_Notarisation-$TIMESTAMP.zip"

# Directories

if [ ! -d "$NOTARIZE_PATH" ]; then
    mkdir "$NOTARIZE_PATH" || exit 1
fi

cd "$EXTERNAL_PATH"

# Find .mxos

LIST=`find *.mxo -depth 0`

# Sign externals and copy to a temp folder

RESULT=0
COMPLETED=0
pids=""

sign()
{
    codesign --force --timestamp --sign FrameLib "$EXTERNAL_PATH/$1" || exit 1
    ditto "$EXTERNAL_PATH/$filename" "$NOTARIZE_PATH/$filename" || exit 1
}

for filename in $LIST
do
    sign "$filename" &
    pids+="$! "
done

# Wait for completion of signing / copying

for pid in $pids; do
    wait $pid
    PRESULT=$?
    if [ $PRESULT -eq 0 ]; then
        let COMPLETED+=1
    else
        RESULT=1
        echo "FAILED - Job $pid exited with a status of $PRESULT"
    fi
done

if [ ! $RESULT -eq 0 ]; then
    echo "FAILED - Some Externals Failed to Sign! $COMPLETED Completed"
    exit 1
fi

# Zip

ditto -c -k --keepParent "$NOTARIZE_PATH" "$ZIP_PATH" || exit 1

# Notarize

if ! xcrun notarytool submit "$ZIP_PATH" --keychain-profile "AJH-NOTARISE" --wait
then
  rm "$ZIP_PATH"
  rm "-r $NOTARIZE_PATH"
  exit 1
fi

rm "$ZIP_PATH"
rm -r "$NOTARIZE_PATH"

# Staple

for filename in $LIST
do
    xcrun stapler staple "$EXTERNAL_PATH/$filename" || exit 1
done

