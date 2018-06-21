from __future__ import absolute_import, division, print_function

def PLUGIN_ENTRY(*args, **kwargs):
    from idaskins.plugin import IdaSkinsPlugin
    return IdaSkinsPlugin(*args, **kwargs)
