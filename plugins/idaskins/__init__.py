from __future__ import absolute_import, print_function, division

import os

VERSION = 'v2.1.0'
PLUGIN_DIR = os.path.dirname(os.path.realpath(__file__))
IDA_DIR = os.path.abspath(os.path.join(PLUGIN_DIR, '..', '..'))
UI_DIR = os.path.join(PLUGIN_DIR, 'ui')
THEMES_DIR = os.path.join(PLUGIN_DIR, 'themes')
