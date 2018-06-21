import idaapi

from idaskins.settings import Settings
from idaskins.objectinspector import ObjectInspector
from idaskins.themeselector import ThemeSelector
from PyQt5.QtWidgets import QMessageBox
from PyQt5.Qt import qApp


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


class IdaSkinsPlugin(idaapi.plugin_t):
    flags = idaapi.PLUGIN_FIX
    comment = "Advanced IDA skinning"

    help = "This is help"
    wanted_name = "IDASkins: Settings"
    wanted_hotkey = "Ctrl-Shift-S"

    def __init__(self, *args, **kwargs):
        idaapi.msg("[IDASkins] v2.0 by athre0z (zyantific.com) loaded!\n")

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

        # Init some vars.
        self._theme_selector = None

        # Subscribe UI notifications.
        self._ui_hooks = UiHooks()
        self._ui_hooks.hook()

    def open_theme_selector(self):
        self._theme_selector = ThemeSelector()
        self._theme_selector.show()

    def init(self):
        return idaapi.PLUGIN_KEEP

    def run(self, arg):
        self.open_theme_selector()

    def term(self):
        idaapi.msg("term() called!\n")
