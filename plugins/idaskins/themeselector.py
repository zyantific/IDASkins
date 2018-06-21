from __future__ import absolute_import, print_function, division

from PyQt5.QtWidgets import QDialog, QFileSystemModel
from PyQt5.QtGui import QPixmap
from PyQt5.QtCore import Qt
from PyQt5 import uic

from idaskins import UI_DIR, THEMES_DIR
from idaskins.thememanifest import ThemeManifest, ManifestError
from idaskins.objectinspector import ObjectInspector
import os
import idaapi
import json


Ui_ThemeSelector, ThemeSelectorBase = uic.loadUiType(
    os.path.join(UI_DIR, 'ThemeSelector.ui')
)


class ThemeSelector(ThemeSelectorBase):
    def __init__(self, *args, **kwargs):
        super(ThemeSelector, self).__init__(*args, **kwargs)
        self._theme_list = []
        self._preview_pixmap = None
        self._obj_inspector = None

        self._ui = Ui_ThemeSelector()
        self._ui.setupUi(self)
        self._ui.lwSkinSelection.itemSelectionChanged.connect(
            self.theme_selected
        )
        self._ui.btnObjectInspector.clicked.connect(
            self.open_object_inspector
        )

        self.refresh()

    def refresh(self):
        # Load manifests for all themes.
        try:
            self._theme_list = [
                (
                    os.path.join(THEMES_DIR, x),
                    ThemeManifest(open(
                        os.path.join(THEMES_DIR, x, 'manifest.json')
                    ),
                ))
                for x in os.listdir(THEMES_DIR)
                if os.path.isdir(os.path.join(THEMES_DIR, x))
            ]
        except ManifestError as exc:
            print("Bad manifest:", str(exc))

        # Populate theme list.
        self._ui.lwSkinSelection.clear()
        for _, manifest in self._theme_list:
            self._ui.lwSkinSelection.addItem(manifest.theme_name)

        self._ui.lwSkinSelection.setCurrentIndex(
            self._ui.lwSkinSelection.rootIndex()
        )

    def theme_selected(self):
        sel_model = self._ui.lwSkinSelection.selectionModel()
        sel_indices = sel_model.selectedIndexes()
        theme_dir, manifest = self._theme_list[sel_indices[0].row()]

        self._ui.lblAuthorVal.setText(manifest.author)
        self._ui.lblVersionVal.setText(manifest.version)
        self._ui.lblNotesVal.setText(manifest.notes)
        self._ui.leClrPathVal.setText(
            os.path.join(theme_dir, manifest.clr_file)
            if manifest.clr_file else
            None
        )

        if manifest.preview_image:
            abs_path = os.path.join(theme_dir, manifest.preview_image)
            self._preview_pixmap = QPixmap(abs_path)
            self.update_preview()
        else:
            self._ui.lblPreview.setText('no preview available')

    def update_preview(self):
        if not self._preview_pixmap:
            return

        scaled = self._preview_pixmap.scaled(
            self._ui.lblPreview.width(),
            self._ui.lblPreview.height(),
            Qt.KeepAspectRatio,
            Qt.SmoothTransformation,
        )
        self._ui.lblPreview.setPixmap(scaled)

    def open_object_inspector(self):
        self._obj_inspector = ObjectInspector()
        self._obj_inspector.show()

    def resizeEvent(self, *args, **kwargs):
        super(ThemeSelector, self).resizeEvent(*args, **kwargs)
        self.update_preview()
