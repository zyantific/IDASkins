def PLUGIN_ENTRY(*args, **kwargs):
    from idaskins.plugin import IdaSkinsPlugin
    return IdaSkinsPlugin(*args, **kwargs)
