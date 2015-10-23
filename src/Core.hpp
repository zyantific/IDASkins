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

#ifndef CORE_HPP
#define CORE_HPP

#include "Utils.hpp"
#include "ThemeSelector.hpp"
#include "ObjectInspector.hpp"

#include <QObject>
#include <ida.hpp>

// ============================================================================================= //
// [Core]                                                                                        //
// ============================================================================================= //

/**
 * @brief   Plugin core singleton.
 */
class Core : public QObject, public Utils::Singleton<Core>
{
    Q_OBJECT

    bool m_lastUiActionWasFontChange;
    std::unique_ptr<ThemeSelector> m_themeSelector;
    std::unique_ptr<ObjectInspector> m_objectInspector;
public:
    /**
     * @brief   Default constructor.
     */
    Core();
    /**
     * @brief   Destructor.
     */
    ~Core();
    /**
     * @brief   Runs the plugin.
     */
    void runPlugin();
protected:
    /**
     * @brief   Opens the theme selection dialog.
     */
    void openThemeSelectionDialog();
    /**
     * @brief   Applies the Qt stylesheet.
     * @return  true if it succeeds, false if it fails.
     * @param   themeDir    The directoy of the theme to apply.
     */
    bool applyStylesheet(QDir &themeDir);
    /**
     * @brief   Convenience wrapper around @c applyStylesheet.
     * @return  true if it succeeds, false if it fails.
     *          
     * Applies theme specified in the settings.
     */
    bool applyStylesheetFromSettings();
    /**
     * @brief   Preprocess stylesheet.
     * @param   qss             The stylesheet contents.
     * @param   themeDirPath    Pathname of the theme directory.
     *                          
     * Replaces IDASkins constants with their value.
     */
    void preprocessStylesheet(QString &qss, const QString &themeDirPath);
    /**
     * @brief   IDA user interface event hook.
     * @param   userData            This pointer.
     * @param   notificationCode    The notification code.
     * @param   va                  The variable arguments.
     * @return  Always 0.
     */
    static int idaapi uiHook(void *userData, int notificationCode, va_list va);
protected slots:
    /**
     * @brief   Signal emitted when a theme selection was accepted in
     *          @c openThemeSelectionDialog.
     */
    void onThemeSelectionAccepted();
    /**
     * @brief   Slot emitted when clickin the "Object inspector" button.
     */
    void openObjectInspector();
};

// ============================================================================================= //

#endif
