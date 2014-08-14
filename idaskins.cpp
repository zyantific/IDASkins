/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 athre0z
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Utils.hpp"
#include "Settings.hpp"
#include "ThemeSelector.hpp"
#include "Config.hpp"

#include <QtGui>
#include <QDockWidget>
#include <ida.hpp>
#include <idp.hpp>
#include <loader.hpp>
#include <diskio.hpp>
#include <functional>

// ============================================================================

/**
 * @brief   Applies the Qt stylesheet.
 * @return  true if it succeeds, false if it fails.
 * @param   themeDir    The directoy of the theme to apply.
 */
bool applyStylesheet(QDir themeDir)
{
    QString themeDirPath = themeDir.absolutePath();
    QFile stylesheet(themeDirPath + "/stylesheet.qss");
    if (!stylesheet.open(QFile::ReadOnly))
    {
        msg("["PLUGIN_NAME"] Unable to load stylesheet file.\n");
        return false;
    }

    QString data = stylesheet.readAll();
    data.replace("<IDADIR>", idadir(nullptr));
    data.replace("<SKINDIR>", themeDirPath);
    qApp->setStyleSheet(data);
    msg("["PLUGIN_NAME"] Skin file successfully applied!\n");

    /*
    static bool first = true;
    // Information gathering
    if (!first)
    {
        QFile log(QString(idadir(nullptr)) + "/skin/object_log.log");
        log.open(QFile::WriteOnly);
        std::function<void(QObject*, int)> helper = [&](QObject *element, int depth)
        {
            for (int i = 0; i < depth; ++i)
                log.write("--");
            log.write(element->metaObject()->className());
            log.write(" name: ");
            log.write(element->objectName().toAscii().data());
            if (strcmp(element->metaObject()->className(), "QLabel") == 0)
            {
                log.write("; text: ");
                log.write(((QLabel*)element)->text().toAscii().data());
            }
            if (strcmp(element->metaObject()->className(), "QAbstractButton") == 0)
            {
                log.write("; icon: ");
                log.write(((QAbstractButton*)element)->icon().name().toAscii().data());
            }
            log.write("\n");
            auto children = element->children();
            for (auto it = children.begin(); it != children.end(); ++it)
                helper(*it, depth + 1);
        };
        helper(qApp->activeWindow(), 0);
        log.flush();
        log.close();
    }
    first = false;
    */

    return true;
}

/**
 * @brief   Opens the theme selection dialog.
 */
void openThemeSelectionDialog()
{
    ThemeSelector selector;
    selector.exec();

    // New theme selected? Save to settings.
    if (selector.selectedThemeDir())
    {
        Settings().setValue(Settings::kSelectedThemeDir, 
            selector.selectedThemeDir()->dirName());
    }
}

/**
 * @brief   Initialization callback for IDA.
 * @return  A @c PLUGIN_ constant from loader.hpp.
 */
int idaapi init()
{
    if (!is_idaq()) return PLUGIN_SKIP;
    msg(PLUGIN_NAME" "PLUGIN_TEXTUAL_VERSION" by athre0z/Ende! loaded!\n");

    // If first start with plugin, ask for theme.
    Settings settings;
    QVariant firstStartVar = settings.value(Settings::kFirstStart, true);
    bool firstStart = true;
    if (firstStartVar.canConvert<bool>())
        firstStart = firstStartVar.toBool();
    else
        settings.remove(Settings::kFirstStart);

    if (firstStartVar.toBool())
    {
        auto pressedButton = QMessageBox::information(nullptr, PLUGIN_NAME": First start",
            PLUGIN_NAME" detected that this is you first IDA startup with this plugin "
            "installed. Do you wish to select a theme now?", 
            QMessageBox::Yes | QMessageBox::No);

        if (pressedButton == QMessageBox::Yes)
            openThemeSelectionDialog();

        settings.setValue(Settings::kFirstStart, false);
    }

    QDir activeThemeDir;
    if (Utils::getCurrentThemeDir(activeThemeDir))
        applyStylesheet(activeThemeDir);

    return PLUGIN_KEEP;
}

/**
 * @brief   Shutdown callback for IDA.
 */
void idaapi term()
{
    
}

/**
 * @brief   Run callback for IDA.
 */
void idaapi run(int /*arg*/)
{
    openThemeSelectionDialog();

    QDir activeThemeDir;
    if (Utils::getCurrentThemeDir(activeThemeDir))
        applyStylesheet(activeThemeDir);
}

plugin_t PLUGIN =
{
    IDP_INTERFACE_VERSION,
    0,
    init,
    term,
    run,
    "Advanced IDA skinning",
    "Plugin providing advanced skinning facilities using Qt stylesheets.",
    PLUGIN_NAME": Settings",
    "Ctrl-Shift-S"
};

// ============================================================================