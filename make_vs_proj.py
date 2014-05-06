import re
import subprocess
import os

qt_path = os.environ.get('IDAQTDIR')
if qt_path is None:
    print 'Environment variable IDAQTDIR not set, giving up.'
    exit(1)

subprocess.call([os.path.join(qt_path, 'bin', 'qmake'), '-tp', 'vc'])
with open("idaskins.vcxproj", 'r') as f:
    fc = f.read().split('\n')

payload = '''  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <TargetExt>.plw</TargetExt>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <TargetExt>.plw</TargetExt>
  </PropertyGroup>'''

with open("idaskins.vcxproj", 'w') as f:
    f.write('\n'.join(fc[:-1] + [payload] + fc[-1:]))
