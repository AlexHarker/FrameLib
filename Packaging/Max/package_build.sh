
# Remove single object build

if [ -d "$SRCROOT/Packaging/Max/FrameLib/init/framelib_max.mxo" ]
then
    rm -r "$SRCROOT/Packaging/Max/FrameLib/init/framelib_max.mxo"
fi

# Check for relevant Win externals

# Notarize Mac externals

"$SRCROOT/Packaging/Max/package_notarization.sh" || exit 1

# Zip the package

PACKAGE_PATH="$SRCROOT/Packaging/Max/FrameLib"
ZIP_PATH="$SRCROOT/build/FrameLib.zip"

/usr/bin/ditto -c -k --norsrc --keepParent "$PACKAGE_PATH" "$ZIP_PATH" || exit 1
