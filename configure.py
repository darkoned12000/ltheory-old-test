#!/usr/bin/env python3
# Copyright (C) 2025  darkoned12000
# SPDX-License-Identifier: GPL-3.0-or-later
# Part of the ltheory-old-test modernization effort (Revamp Work).
# See NOTICE and LICENSE.GPL. Original engine (c) Josh Parnell, public domain.
"""Thin wrapper around cmake presets for LTheory builds.

Usage:
    python3 configure.py                # cmake configure (default preset)
    python3 configure.py build          # parallel build
    python3 configure.py clean          # remove bin/ build/ cache/
    python3 configure.py run <app>      # run an LTSL app with LD_LIBRARY_PATH
    python3 configure.py help           # show this help

CMake presets (use directly if preferred):
    cmake --preset default              # configure (RelWithDebInfo, Make)
    cmake --preset debug                # configure (Debug, Make)
    cmake --preset ninja                # configure (RelWithDebInfo, Ninja)
    cmake --preset ninja-debug          # configure (Debug, Ninja)
    cmake --build --preset default      # build (default preset)
"""

import os
import shutil
import subprocess
import sys


def run_cmake_configure(preset="default"):
    return subprocess.run(["cmake", "--preset", preset]).returncode


def run_cmake_build(preset="default"):
    return subprocess.run(["cmake", "--build", "--preset", preset]).returncode


def run_clean():
    for d in ("bin", "build", "cache"):
        shutil.rmtree(d, ignore_errors=True)


def run_app(args):
    if not args:
        print("Usage: configure.py run <app-name>")
        print("Example: configure.py run war")
        sys.exit(1)

    if sys.platform == "win32":
        exe = "bin/launch.exe"
    else:
        exe = "bin/launch"

    env = dict(os.environ)
    if sys.platform != "win32":
        paths = [
            os.path.join(os.getcwd(), "bin"),
            os.path.join(os.getcwd(), "extbin", "linux64"),
        ]
        existing = env.get("LD_LIBRARY_PATH", "")
        env["LD_LIBRARY_PATH"] = os.pathsep.join(
            paths + existing.split(os.pathsep) if existing else paths
        )

    return subprocess.run([exe] + args, env=env).returncode


def main():
    args = sys.argv[1:]

    if not args or args[0] in ("configure", "setup"):
        preset = args[1] if len(args) > 1 else "default"
        print(f"[configure.py] Configuring with preset: {preset}")
        sys.exit(run_cmake_configure(preset))

    elif args[0] == "build":
        preset = args[1] if len(args) > 1 else "default"
        print(f"[configure.py] Building with preset: {preset}")
        sys.exit(run_cmake_build(preset))

    elif args[0] == "clean":
        run_clean()

    elif args[0] == "run":
        sys.exit(run_app(args[1:]))

    elif args[0] in ("help", "--help", "-h"):
        print(__doc__.strip())

    else:
        print(f"Unknown command: {args[0]}")
        print("Commands: configure [preset], build [preset], clean, run <app>, help")
        sys.exit(1)


if __name__ == "__main__":
    main()
