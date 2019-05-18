from __future__ import absolute_import, division, print_function

import os
import sys
import ctypes
import traceback

import idaapi
import idc

from ctypes import (c_int, c_void_p, create_string_buffer, cast)
from PyQt5.QtCore import Qt, QTimer, QObject
from PyQt5.QtGui import QShowEvent
from PyQt5.QtWidgets import QWidget, QDialog, QDialogButtonBox, QPushButton, qApp

__all__ = ['load_clr_file']


def _ida_lib():
    ea_name = 'ida64' if idc.__EA64__ else 'ida'
    if sys.platform == 'win32':
        functype = ctypes.WINFUNCTYPE
        lib = ctypes.WinDLL(ea_name)
    elif sys.platform == 'darwin':
        functype = ctypes.CFUNCTYPE
        lib = ctypes.CDLL(idaapi.idadir("lib" + ea_name + ".dylib"))
    else:
        functype = ctypes.CFUNCTYPE
        lib = ctypes.CDLL('lib' + ea_name + '.so')
    return functype, lib


functype, lib = _ida_lib()
hook_cb_t = functype(c_void_p, c_void_p, c_int, c_void_p)

hook_to_notification_point = lib.hook_to_notification_point
hook_to_notification_point.argtypes = [c_int, hook_cb_t, c_void_p]

unhook_from_notification_point = lib.unhook_from_notification_point
unhook_from_notification_point.argtypes = [c_int, hook_cb_t, c_void_p]


class TemporaryFilter(QObject):
    """
    Temporary event filter installed at qApp to catch events
    while executing QDialog::exec.

    The filter automatically clicks &Import button,
    and automatically selects file by using native ui hooks.
    """

    def __init__(self, filepath):
        super(TemporaryFilter, self).__init__()
        filepath = os.path.abspath(filepath)
        if not os.path.isfile(filepath):
            raise IOError("Assertion Error: os.path.isfile(filepath)")

        self.filepath = filepath

    def eventFilter(self, obj, event):
        def is_colors_dialog():
            return isinstance(
                obj, QDialog) and 'IDA Colors' in obj.windowTitle()

        if isinstance(event, QShowEvent) and is_colors_dialog():
            qApp.removeEventFilter(self)

            # Hide window and find &Import button
            obj.windowHandle().setOpacity(0)
            buttons = [widget for widget in obj.children() if isinstance(
                widget, QDialogButtonBox)][0]
            button = [widget for widget in buttons.buttons() if widget.text()
                      == '&Import'][0]

            with NativeHook(ask_file=self.ask_file_handler):
                button.click()

            QTimer.singleShot(0, lambda: obj.accept())
            return 1
        return 0

    def ask_file_handler(self):
        return create_string_buffer(self.filepath)


class NativeHook:
    """
    Installer for non-exposed hooks from UI_Hooks.
    This uses hook_to_notification_point with HT_UI.

    with NativeHook(ask_file=lambda: 0):
        # do anything
    """
    NAMES = {
        'ask_file': 0x1d
    }
    HT_UI = 1

    def __init__(self, **kwargs):
        self.hooks = {NativeHook.NAMES[key]: value for key, value in kwargs.items()}
        self._handler = hook_cb_t(self.handler)

    def handler(self, _user_data, code, _va_args):
        if code in self.hooks:
            try:
                res = self.hooks[code]()
                return cast(res, c_void_p).value
            except:
                traceback.print_exc()
                return 0
        else:
            return 0

    def __enter__(self):
        hook_to_notification_point(NativeHook.HT_UI, self._handler, None)

    def __exit__(self, *args):
        unhook_from_notification_point(NativeHook.HT_UI, self._handler, None)


def load_clr_file(filepath):
    event_filter = TemporaryFilter(filepath)
    qApp.installEventFilter(event_filter)

    return idaapi.process_ui_action('SetColors')
