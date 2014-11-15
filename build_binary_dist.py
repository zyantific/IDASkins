#!/usr/bin/env python

"""
    This script build the binary distribution for the Windows version of IDA
    PRO for multiple IDA versions in one batch.

    The MIT License (MIT)

    Copyright (c) 2014 athre0z
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
"""

import os
import errno
import argparse

from subprocess import Popen, PIPE
from distutils.spawn import find_executable

if __name__ == '__main__':
    #
    # Parse arguments
    #
    parser = argparse.ArgumentParser(
        description='Batch build script creating the plugin for multiple IDA versions')
    parser.add_argument('ida_sdks_path', type=str, 
        help='Path containing the IDA SDKs for the desired IDA target versions')
    parser.add_argument('--cmake_args', default='', type=str,
        help='Additional arguments passed to cmake', nargs='?')
    parser.add_argument('target_versions', metavar='target_version', type=str, 
        nargs='+')
    args = parser.parse_args()

    def print_usage(error=None):
        parser.print_usage()
        if error:
            print(error)
        exit()

    target_versions = []
    for cur_version in args.target_versions:
        cur_version = cur_version.strip().split('.')
        try:
            target_versions.append((int(cur_version[0]), int(cur_version[1])))
        except (ValueError, IndexError):
            print_usage('[-] Invalid version format, expected something like "6.5"')

    #
    # Find tools
    #
    cmake_bin = find_executable('cmake')
    msbuild_bin = find_executable('MSBuild')
    if not cmake_bin:
        print_usage('[-] Unable to find cmake binary')
    if not msbuild_bin:
        print_usage('[-] Unable to find MSBuild (please use Visual Studio CMD)')

    #
    # Build targets
    #
    for arch in (32, 64):
        for cur_target in target_versions:
            build_dir = 'build-{}.{}-{}'.format(*(cur_target + (arch,)))
            try:
                os.mkdir(build_dir)
            except OSError as e:
                if e.errno != errno.EEXIST:
                    raise

            proc = Popen([
                cmake_bin, 
                '-Dida_sdk=' + os.path.join(args.ida_sdks_path, 'idasdk{}{}'.format(*cur_target)),
                '-G', 'Visual Studio 10',
                '-DCMAKE_INSTALL_PREFIX:PATH=../dist/IDA-{}.{}'.format(*cur_target),
                ] + args.cmake_args.split(' ') + ['..'] + (
                    ['-DIDA_ARCH_64=TRUE'] if arch == 64 else []
                ), cwd=build_dir)
            if proc.wait() != 0:
                print('[-] CMake failed, giving up.')
                exit()

            proc = Popen([msbuild_bin, 'IDASkins.sln', '/p:Configuration=Release'], cwd=build_dir)
            if proc.wait() != 0:
                print('[-] MSBuild failed, giving up.')
                exit()

            proc = Popen([msbuild_bin, 'INSTALL.vcxproj', '/p:Configuration=Release'], 
                cwd=build_dir)
            if proc.wait() != 0:
                print('[-] MSBuild INSTALL failed, giving up.')
                exit()

    print('[+] Done!')