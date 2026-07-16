#!/usr/bin/env python
# Copyright (C) 2025  darkoned12000
# SPDX-License-Identifier: GPL-3.0-or-later
# Part of the ltheory-old-test modernization effort (Revamp Work).
# See NOTICE and LICENSE.GPL. Original engine (c) Josh Parnell, public domain.
import os
import sys
import shutil
import subprocess

try:
    os.mkdir('build')
except Exception as e:
    pass

if len(sys.argv) > 1:
    if sys.argv[1] == 'build':
        import multiprocessing
        jobs = multiprocessing.cpu_count()
        subprocess.call(['cmake', '--build', './build', '-j', str(jobs)])
    elif sys.argv[1] == 'clean':
        shutil.rmtree('bin', ignore_errors = True)
        shutil.rmtree('build', ignore_errors = True)
        shutil.rmtree('cache', ignore_errors = True)
    elif sys.argv[1] == 'run':
        import platform
        exe = 'bin/launch.exe' if platform.system() == 'Windows' else 'bin/launch'
        env = dict(os.environ)
        if platform.system() != 'Windows':
            # Make the bundled FMOD/ENet runtime libs reachable
            paths = [os.path.join(os.getcwd(), 'bin'),
                     os.path.join(os.getcwd(), 'extbin', 'linux64')]
            env['LD_LIBRARY_PATH'] = os.pathsep.join(
                paths + env.get('LD_LIBRARY_PATH', '').split(os.pathsep)) if env.get('LD_LIBRARY_PATH') else os.pathsep.join(paths)
        subprocess.call([exe] + sys.argv[2:], env=env)
else:
    subprocess.call(['cmake', '-S', './', '-B', './build', '-DCMAKE_BUILD_TYPE=RelWithDebInfo'])
