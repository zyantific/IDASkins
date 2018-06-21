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


_required_keys = {'qss_file', 'theme_name', 'version', 'author'}


class ThemeManifest(object):
    """Theme manifest. Read access to basic info about themes."""
    def __init__(self, file):
        try:
            self._data = json.load(file)
        except (TypeError, ValueError) as exc:
            raise ManifestError('Bad manifest: ' + str(exc))

        missing_keys = _required_keys - set(self._data.keys())
        if missing_keys:
            raise ManifestError(
                'Bad manifest: missing required key(s): {}'.format(
                    ', '.join(missing_keys)
                )
            )


    # Required.
    theme_name = _make_property('theme_name')
    author = _make_property('author')
    version = _make_property('version')
    qss_file = _make_property('qss_file')

    # Optional.
    preview_image = _make_property('preview_image')
    notes = _make_property('notes')
    clr_file = _make_property('clr_file')
