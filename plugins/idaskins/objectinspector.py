from PyQt5.QtWidgets import QWidget
from PyQt5 import uic
from idaskins import UI_DIR
import os


Ui_ObjectInspector, ObjectInspectorBase = uic.loadUiType(
    os.path.join(UI_DIR, 'ObjectInspector.ui')
)


class ObjectInspector(ObjectInspectorBase):
    pass
