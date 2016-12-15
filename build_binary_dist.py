#!/usr/bin/env python

"""
    This script builds the binary distribution for multiple IDA versions in
    one batch.

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


def get_build_cmd(platform):
    return {'mac': 'make', 'unix': 'make', 'win': 'MSBuild'}[platform]


def get_cmake_gen(platform, cur_target):
    if platform == 'unix' or platform == 'mac':
        return 'Unix Makefiles'
    elif platform == 'win':
        return 'Visual Studio ' + ('10' if cur_target[0] <= 6 and cur_target[1] <= 8 else '14')
    else:
        raise Exception('Unknown platform "%s"' % platform)


def get_build_solution_arguments(platform):
    build_bin = get_build_cmd(platform)
    if platform == 'win':
        return [build_bin, 'IDASkins.sln', '/p:Configuration=Release']
    elif platform == 'unix' or platform == 'mac':
        return [build_bin]
    else:
        raise Exception('Unknown platform "%s"' % platform)


def get_install_solution_arguments(platform):
    build_bin = get_build_cmd(platform)
    if platform == 'win':
        return [build_bin, 'INSTALL.vcxproj', '/p:Configuration=Release']
    elif platform == 'unix' or platform == 'mac':
        return [build_bin, 'install', 'VERBOSE=1']
    else:
        raise Exception('Unrecognized platform "%s"' % platform)


if __name__ == '__main__':
    #
    # Parse arguments
    #
    parser = argparse.ArgumentParser(
            description='Batch build script creating the plugin for multiple IDA versions.')

    target_args = parser.add_argument_group('target configuration')
    target_args.add_argument('--ida-sdks-path', '-i', type=str, required=True,
            help='Path containing the IDA SDKs for the desired IDA target versions')
    target_args.add_argument('--ida-path', '-I', type=str, required=True,
            help='Path containing installed IDA for the desired IDA target versions')
    target_args.add_argument('--platform', '-p', type=str, choices=['win', 'unix', 'mac'],
            help='Platform to build for (e.g. win, unix)', required=True)
    target_args.add_argument('--target-version', '-t', action='append', required=True,
            help='IDA versions to build for (e.g. 6.7). May be passed multiple times.')

    parser.add_argument('--skip-install', action='store_true', default=False,
            help='Do not execute install target')
    parser.add_argument('cmake_args', default='', type=str,
            help='Additional arguments passed to CMake', nargs=argparse.REMAINDER)
    args = parser.parse_args()

    def print_usage(error=None):
        parser.print_usage()
        if error:
            print(error)
        exit()

    target_versions = []
    for cur_version in args.target_version:
        cur_version = cur_version.strip().split('.')
        try:
            target_versions.append((int(cur_version[0]), int(cur_version[1])))
        except (ValueError, IndexError):
            print_usage('[-] Invalid version format, expected something like "6.5"')

    #
    # Find tools
    #
    cmake_bin = find_executable('cmake')
    build_bin = find_executable(get_build_cmd(args.platform))
    if not cmake_bin:
        print_usage('[-] Unable to find cmake binary')
    if not build_bin:
        print_usage('[-] Unable to find build (please use Visual Studio MSBuild CMD or make)')

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


            # Run cmake
            cmake_cmd = [cmake_bin,
                '-Dida_sdk=' + os.path.join(args.ida_sdks_path, 'idasdk{}{}'.format(*cur_target)),
                '-Dida_dir_env=' + os.path.join(args.ida_path),
                '-G', get_cmake_gen(args.platform, cur_target),
                '-DIDA_INSTALL_DIR:PATH=../dist/IDA-{}.{}'.format(*cur_target),
                '-DIDA_VERSION={}{}0'.format(*cur_target)
                ] + args.cmake_args + ['..'] + (
                    ['-DIDA_ARCH_64=TRUE'] if arch == 64 else []
                )
            print 'Cmake command:'
            print ' '.join("'%s'" % x if ' ' in x else x for x in cmake_cmd)

            proc = Popen(cmake_cmd, cwd=build_dir)
            if proc.wait() != 0:
                print('[-] CMake failed, giving up.')
                exit()


            # Build plugin
            proc = Popen(get_build_solution_arguments(args.platform), cwd=build_dir)
            if proc.wait() != 0:
                print('[-] Build failed, giving up.')
                exit()

            if not args.skip_install:
                # Install plugin
                proc = Popen(get_install_solution_arguments(args.platform),
                    cwd=build_dir)
                if proc.wait() != 0:
                    print('[-] Install failed, giving up.')
                    exit()

    print('[+] Done!')
