"""
     Generic python script generating Visual Studio project files from IDA PRO
     plugin qmake project files.

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

import subprocess
import os

config = {
    # User configuration ------------------------------------------------------

    # Arguments passed to the IDA process when debugged.
    # None = no debug configuration is generated.
    'debug_args': '"C:\Users\Ende\Desktop\putty.idb"',

    # -------------------------------------------------------------------------

    # Project configuration ---------------------------------------------------
    'project_name': 'IDASkins',
    'project_file_name': 'idaskins.pro'
    # -------------------------------------------------------------------------
}


def make_vs_proj(is64, cfg):
    qt_path = os.environ.get('IDAQTDIR')
    ida_dir = os.environ.get('IDADIR')
    if None in (qt_path, ida_dir):
        print 'Environment variable IDAQTDIR or IDADIR not set, giving up.'
        return False

    qmake_result = subprocess.call([os.path.join(qt_path, 'bin', 'qmake'), '-tp', 'vc',
                                    cfg['project_file_name']])

    if qmake_result != 0:
        print 'qmake failed, giving up.'
        return False

    vcxproj_additions = '''<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
          <TargetExt>{}</TargetExt>
        </PropertyGroup>
        <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
          <TargetExt>{}</TargetExt>
        </PropertyGroup>'''\
        .format(*((('.p64' if is64 else '.plw'),) * 2))

    vcxproj_file = cfg['project_name'] + '.vcxproj'
    with open(vcxproj_file, 'r') as f:
        fc = f.read().split('\n')
    with open(vcxproj_file, 'w') as f:
        f.write('\n'.join(fc[:-1] + [vcxproj_additions] + fc[-1:]))

    if cfg['debug_args'] is not None:
        user_file_content = '''<?xml version="1.0" encoding="utf-8"?>
            <Project ToolsVersion="10.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
              <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
                <LocalDebuggerCommand>{}\{}.exe</LocalDebuggerCommand>
                <LocalDebuggerCommandArguments>{}</LocalDebuggerCommandArguments>
                <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
              </PropertyGroup>
              <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
                <LocalDebuggerCommand>{}\{}.exe</LocalDebuggerCommand>
                <LocalDebuggerCommandArguments>{}</LocalDebuggerCommandArguments>
                <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>
              </PropertyGroup>
            </Project>'''\
            .format(*((ida_dir, ('idaq64' if is64 else 'idaq'), cfg['debug_args']) * 2))

        user_file = cfg['project_name'] + '.vcxproj.user'
        with open(user_file, 'w') as f:
            f.write(user_file_content)

    return True


if __name__ == '__main__':
    make_vs_proj(False, config)
