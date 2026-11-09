import argparse
import os
from pathlib import Path
import subprocess
import sys


ROOT = Path(__file__).resolve().parent.parent
SOURCE = ROOT / "FrameLib_Dependencies" / "foonathan-memory"
BUILD = ROOT / "build" / "flucoma"


def build(name, configuration, extra):
    directory = BUILD / name
    subprocess.run([
        "cmake", "-S", str(SOURCE), "-B", str(directory),
        "-DCMAKE_BUILD_TYPE=" + configuration,
        "-DFOONATHAN_MEMORY_BUILD_TESTS=OFF",
        "-DFOONATHAN_MEMORY_BUILD_EXAMPLES=OFF",
        "-DFOONATHAN_MEMORY_BUILD_TOOLS=OFF",
        *extra,
    ], check=True)
    subprocess.run(["cmake", "--build", str(directory), "--config", configuration], check=True)


def main():
    parser = argparse.ArgumentParser(description="Build the foonathan memory dependency for FrameLib")
    parser.add_argument("--platform", choices=("x64", "Win32"), default="x64",
                        help="Visual Studio platform (Windows only)")
    args = parser.parse_args()
    if not (SOURCE / "CMakeLists.txt").is_file():
        parser.error("initialise submodules first: git submodule update --init --recursive")
    if sys.platform == "darwin":
        build("macos", "Release", [
            "-DCMAKE_OSX_ARCHITECTURES=x86_64;arm64",
            "-DCMAKE_OSX_DEPLOYMENT_TARGET=10.9",
        ])
    elif os.name == "nt":
        for configuration in ("Debug", "Release"):
            build("windows-{}-{}".format(args.platform, configuration), configuration,
                  ["-A", args.platform])
        build("windows-{}-Static".format(args.platform), "Release",
              ["-A", args.platform, "-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded"])
    else:
        parser.error("use the PD makefile to build the dependency on this platform")


if __name__ == "__main__":
    main()
