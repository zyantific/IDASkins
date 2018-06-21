import json


class ManifestError(Exception):
    pass


class ThemeManifest(object):
    def __init__(self, file):
        try:
            self._data = json.load(file)
        except (TypeError, ValueError) as exc:
            raise ManifestError("Bad manifest: " + str(exc))

