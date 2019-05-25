from __future__ import absolute_import, division, print_function

import os

import idaapi
from idaskins import IDA_DIR, THEMES_DIR, VERSION
from idaskins.idafontconfig import IdaFontConfig
from idaskins.objectinspector import ObjectInspector
from idaskins.settings import Settings
from idaskins.thememanifest import ThemeManifest, ManifestError
from idaskins.themeselector import ThemeSelector
from idaskins.clrapplier import load_clr_file
from PyQt5.Qt import qApp
from PyQt5.QtCore import QObject, QDir
from PyQt5.QtWidgets import QMessageBox


class UiHooks(idaapi.UI_Hooks):
    """
    UI hooks. Currently only used to display a warning when
    switching font settings in IDA.
    """
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
                "Please note that altering the font settings when IDASkins "
                "is loaded may cause strange effects on font rendering. It is "
                "recommended to restart IDA after making font-related changes "
                "in the settings to avoid instability."
            )

        return super(UiHooks, self).postprocess_action()


class IdaSkinsPlugin(QObject, idaapi.plugin_t):
    """Plugin entry point. Does most of the skinning magic."""
    flags = idaapi.PLUGIN_FIX
    comment = "Advanced IDA skinning"

    help = "This is help"
    wanted_name = "IDASkins: Settings"
    wanted_hotkey = "Ctrl-Shift-S"

    def __init__(self, *args, **kwargs):
        print("[IDASkins] {} by athre0z (zyantific.com) loaded!".format(
            VERSION
        ))

        QObject.__init__(self, *args, **kwargs)
        idaapi.plugin_t.__init__(self)

        # First start dialog.
        self._settings = Settings()
        if self._settings.first_start:
            selection = QMessageBox.information(
                qApp.activeWindow(),
                "IDASkins: First start",
                "IDASkins has detected that this is the first time you've started IDA with "
                "the plugin installed. Select a theme now?",
                QMessageBox.Yes | QMessageBox.No,
            )

            if selection == QMessageBox.Yes:
                self.open_theme_selector()

            self._settings.first_start = False
        else:
            # v2.0.0 used absolute pathes due to a bug.
            # Fix settings from this particular version here.
            theme_dir = self._settings.selected_theme_dir
            if theme_dir and os.path.isabs(theme_dir):
                print('[IDASkins] Updating buggy v2.0.0 theme path')
                self._settings.selected_theme_dir = os.path.split(theme_dir)[-1]

        self._theme_selector = None
        self.apply_stylesheet_from_settings()

        # Subscribe UI notifications.
        self._ui_hooks = UiHooks()
        self._ui_hooks.hook()

    def preprocess_stylesheet(self, qss, abs_theme_dir):
        qss = qss.replace('<IDADIR>', QDir.fromNativeSeparators(IDA_DIR))
        qss = qss.replace('<SKINDIR>', QDir.fromNativeSeparators(abs_theme_dir))

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

    def apply_stylesheet(self, abs_theme_dir, manifest):
        try:
            with open(os.path.join(abs_theme_dir, manifest.qss_file)) as f:
                qss = f.read()
        except IOError as exc:
            print('[IDASkins] Unable to load stylesheet.')
            return

        qss = self.preprocess_stylesheet(qss, abs_theme_dir)
        qApp.setStyleSheet(qss)
        #idaapi.request_refresh(idaapi.IWID_ALL)

    def apply_clr_file(self, abs_theme_dir, manifest):
        try:
            load_clr_file(os.path.join(abs_theme_dir, manifest.clr_file))
        except IOError as exc:
            print('[IDASkins] Unable to load clr file.')
            return

    def apply_stylesheet_from_settings(self):
        theme_dir = self._settings.selected_theme_dir
        if theme_dir:
            abs_theme_dir = os.path.join(THEMES_DIR, theme_dir)
            try:
                manifest = ThemeManifest(open(os.path.join(
                    abs_theme_dir, 'manifest.json'
                )))
            except ManifestError as exc:
                print('[IDASkins]', str(exc))
                return

            self.apply_stylesheet(abs_theme_dir, manifest)
            self.apply_clr_file(abs_theme_dir, manifest)
            print('[IDASkins] Skin file successfully applied!')

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
