from __future__ import absolute_import, division, print_function

from PyQt5.QtCore import QSettings

SELECTED_THEME_DIR_KEY = 'selectedThemeDir'
FIRST_START_KEY = 'firstStart'


class Settings(object):
    def __init__(self):
        self._settings = QSettings("athre0z", "IDASkins")

    @property
    def selected_theme_dir(self):
        theme_dir = self._settings.value(SELECTED_THEME_DIR_KEY, None)
        return None if theme_dir is None else theme_dir.encode()

    @selected_theme_dir.setter
    def selected_theme_dir(self, v):
        assert isinstance(v, str)
        self._settings.setValue(SELECTED_THEME_DIR_KEY, v)

    @property
    def first_start(self):
        # PyQt seems to be returning unicode strings instead of QVariant.
        return not self._settings.value(FIRST_START_KEY, True) in [u'false', False]

    @first_start.setter
    def first_start(self, v):
        assert isinstance(v, bool)
        self._settings.setValue(FIRST_START_KEY, v)
