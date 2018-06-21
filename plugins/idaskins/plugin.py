from __future__ import absolute_import, print_function, division

import idaapi
import os

from idaskins.settings import Settings
from idaskins.objectinspector import ObjectInspector
from idaskins.themeselector import ThemeSelector
from idaskins.idafontconfig import IdaFontConfig

from PyQt5.QtWidgets import QMessageBox
from PyQt5.Qt import qApp
from PyQt5.QtCore import QObject


class UiHooks(idaapi.UI_Hooks):
    def __init__(self):
        super(UiHooks, self).__init__()
        self._last_event = None

    def preprocess_action(self, name):
        self._last_event = name
        return super(UiHooks, self).preprocess_action(name)

    def postprocess_action(self):
        if self._last_event == 'SetFont':
            QMessageBox.warning(
                qApp.activeWindow(),
                "IDASkins",
                "Please note that altering the font settings when IDASkins is loaded "
                "may cause strange effects on font rendering. It is recommended to "
                "restart IDA after making font-related changes in the settings to avoid "
                "instability."
            )

        return super(UiHooks, self).postprocess_action()


class IdaSkinsPlugin(QObject, idaapi.plugin_t):
    flags = idaapi.PLUGIN_FIX
    comment = "Advanced IDA skinning"

    help = "This is help"
    wanted_name = "IDASkins: Settings"
    wanted_hotkey = "Ctrl-Shift-S"

    def __init__(self, *args, **kwargs):
        QObject.__init__(self, *args, **kwargs)
        idaapi.plugin_t.__init__(self)

        print("[IDASkins] v2.0 by athre0z (zyantific.com) loaded!")

        # First start dialog.
        self._settings = Settings()
        if self._settings.first_start:
            selection = QMessageBox.information(
                qApp.activeWindow(),
                "IDASkins: First start",
                "IDASkins detected that this is you first IDA startup with this plugin "
                "installed. Do you wish to select a theme now?",
                QMessageBox.Yes | QMessageBox.No,
            )

            if selection == QMessageBox.Yes:
                self.open_theme_selector()

            self._settings.first_start = False

        self._theme_selector = None
        self.apply_stylesheet_from_settings()

        # Subscribe UI notifications.
        self._ui_hooks = UiHooks()
        self._ui_hooks.hook()

    def preprocess_stylesheet(self, qss, theme_dir):
        qss = qss.replace('<IDADIR>', idaapi.idadir(None))
        qss = qss.replace('<SKINDIR>', theme_dir)

        def replace_keyword(x, keyword, kind):
            cfg = IdaFontConfig(kind)
            prefix = '<{}_FONT_'.format(keyword)

            x = x.replace(prefix + 'FAMILY>', cfg.family)
            x = x.replace(prefix + 'STYLE>', ' italic' if cfg.italic else '')
            x = x.replace(prefix + 'WEIGHT>', ' bold' if cfg.bold else '')
            x = x.replace(prefix + 'SIZE>', '{} pt'.format(cfg.size))

            return x

        qss = replace_keyword(qss, 'DISASSEMBLY', 'disas')
        qss = replace_keyword(qss, 'HEXVIEW', 'hexview')
        qss = replace_keyword(qss, 'DEBUG_REGISTERS', 'debug_regs')
        qss = replace_keyword(qss, 'TEXT_INPUT', 'text_input')
        qss = replace_keyword(qss, 'OUTPUT_WINDOW', 'output_wnd')

        return qss

    def apply_stylesheet(self, theme_dir):
        try:
            with open(os.path.join(theme_dir, 'stylesheet.qss')) as f:
                qss = f.read()
        except IOError as exc:
            print('[IDASkins] Unable to load stylesheet.')
            return

        qss = self.preprocess_stylesheet(qss, theme_dir)
        qApp.setStyleSheet(qss)
        #idaapi.request_refresh(idaapi.IWID_ALL)
        print('[IDASkins] Skin file successfully applied!')

    def apply_stylesheet_from_settings(self):
        if self._settings.selected_theme_dir:
            self.apply_stylesheet(self._settings.selected_theme_dir)

    def open_theme_selector(self):
        self._theme_selector = ThemeSelector(qApp.activeWindow())
        self._theme_selector.accepted.connect(self.on_theme_selection_accepted)
        self._theme_selector.show()

    def on_theme_selection_accepted(self):
        theme_dir = self.sender().selected_theme_dir
        if theme_dir:
            self._settings.selected_theme_dir = theme_dir
            self.apply_stylesheet_from_settings()

    def init(self):
        return idaapi.PLUGIN_KEEP

    def run(self, arg):
        self.open_theme_selector()

    def term(self):
        print("term() called!")
