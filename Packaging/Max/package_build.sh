
# Remove single object build

if [ -d "$SRCROOT/Packaging/Max/FrameLib/init/framelib_max.mxo" ]
then
    rm -r "$SRCROOT/Packaging/Max/FrameLib/init/framelib_max.mxo"
fi

# Check for relevant Win externals

# Include licenses for the dependencies used by fl.yinfft~.

LICENSES="$SRCROOT/Packaging/Max/FrameLib/misc/licenses"
mkdir -p "$LICENSES" || exit 1
cp "$SRCROOT/FrameLib_Dependencies/flucoma-core/license.md" "$LICENSES/flucoma-core.txt" || exit 1
cp "$SRCROOT/FrameLib_Dependencies/eigen/COPYING.MPL2" "$LICENSES/eigen-MPL2.txt" || exit 1
cp "$SRCROOT/FrameLib_Dependencies/foonathan-memory/LICENSE" "$LICENSES/foonathan-memory.txt" || exit 1

# Notarize Mac externals

"$SRCROOT/Packaging/Max/package_notarization.sh" || exit 1

# Zip the package

PACKAGE_PATH="$SRCROOT/Packaging/Max/FrameLib"
ZIP_PATH="$SRCROOT/build/FrameLib.zip"

/usr/bin/ditto -c -k --norsrc --keepParent "$PACKAGE_PATH" "$ZIP_PATH" || exit 1
