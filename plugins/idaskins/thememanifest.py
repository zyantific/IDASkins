from __future__ import absolute_import, division, print_function

import json


class ManifestError(Exception):
    """Something is wrong with the manifest format."""
    pass


def _make_property(name):
    @property
    def getter(self):
        return self._data.get(name)
    return getter


class ThemeManifest(object):
    """Theme manifest. Read access to basic info about themes."""
    def __init__(self, file):
        try:
            self._data = json.load(file)
        except (TypeError, ValueError) as exc:
            raise ManifestError("Bad manifest: " + str(exc))

    theme_name = _make_property('theme_name')
    author = _make_property('author')
    version = _make_property('version')
    preview_image = _make_property('preview_image')
    notes = _make_property('notes')
    clr_file = _make_property('clr_file')
