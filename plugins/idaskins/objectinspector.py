from __future__ import absolute_import, division, print_function

import os

from idaskins import UI_DIR
from PyQt5 import uic
from PyQt5.Qt import qApp
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QCursor, QFont, QKeySequence
from PyQt5.QtWidgets import QShortcut, QWidget

Ui_ObjectInspector, ObjectInspectorBase = uic.loadUiType(
    os.path.join(UI_DIR, 'ObjectInspector.ui')
)


class ObjectInspector(ObjectInspectorBase):
    """
    Rudimentary Qt object inspector.
    Allows for easier finding of object names and classes
    for usage in QSS stylesheets.
    """
    def __init__(self, *args, **kwargs):
        super(ObjectInspector, self).__init__(*args, **kwargs)

        self._selected_widget = None
        self._ui = Ui_ObjectInspector()
        self._ui.setupUi(self)

        # Make everything monospace.
        font = QFont('Monospace')
        font.setStyleHint(QFont.TypeWriter)
        self._ui.teInspectionResults.setFont(font)

        # Register signals.
        self._update_key = QShortcut(QKeySequence(Qt.Key_F7), self)
        self._ui.btnSelectParent.released.connect(self.select_parent)
        self._update_key.activated.connect(self.update_inspection)

    def update_inspection(self):
        widget = qApp.widgetAt(QCursor.pos())
        self.update_selected_widget(widget)

    def select_parent(self):
        if self._selected_widget:
            parent = self._selected_widget.parent()
            if parent and parent.inherits('QWidget'):
                self.update_selected_widget(parent)

    def update_selected_widget(self, widget):
        if self._selected_widget:
            self._selected_widget.destroyed.disconnect(
                self.on_selected_widget_destroyed
            )

        self._selected_widget = widget

        if widget:
            self._ui.btnSelectParent.setEnabled(widget.parent() is not None)
            self._ui.teInspectionResults.setText((
                "Type: {}\n"
                "Name: {}\n"
                "Number of children: {}\n"
                "QSS: {}"
            ).format(
                widget.metaObject().className(),
                widget.objectName() or '<none>',
                len(widget.children()),
                widget.styleSheet() or '<none>',
            ))

            self._selected_widget.destroyed.connect(
                self.on_selected_widget_destroyed
            )
        else:
            self._ui.teInspectionResults.setText('<no object under cursor>')

    def on_selected_widget_destroyed(self, obj):
        self._selected_widget = None
