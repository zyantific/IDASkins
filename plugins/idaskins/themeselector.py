from PyQt5.QtWidgets import QDialog, QFileSystemModel
from PyQt5 import uic

from idaskins import UI_DIR, THEMES_DIR
from idaskins.thememanifest import ThemeManifest
import os
import json


Ui_ThemeSelector, ThemeSelectorBase = uic.loadUiType(
    os.path.join(UI_DIR, 'ThemeSelector.ui')
)


class ThemeSelector(ThemeSelectorBase):
    def __init__(self, *args, **kwargs):
        super(ThemeSelector, self).__init__(*args, **kwargs)

        self._theme_list = []

        self._ui = Ui_ThemeSelector()
        self._ui.setupUi(self)

    def refresh(self):
        try:
            self._theme_list = [
                ThemeManifest(open(x))
                for x in os.path.listdir(THEMES_DIR)
                if os.path.isdir(x)
            ]
        except (TypeError, ValueError):
            pass

