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

#ifndef THEMESELECTOR_HPP
#define THEMESELECTOR_HPP

#include "ui_ThemeSelector.h"
#include "ThemeManifest.hpp"

#include <memory>
#include <QDir>

// ============================================================================================= //
// [ThemeSelector]                                                                               //
// ============================================================================================= //

/**
 * @brief   Theme selection dialog.
 */
class ThemeSelector : public QDialog
{
    Q_OBJECT
    typedef std::vector<std::pair<QDir, std::unique_ptr<ThemeManifest>>> ThemeList;
    Ui::ThemeSelector m_widgets;
    std::unique_ptr<ThemeList> m_curThemeList;
    QPixmap m_curPreviewImage;
public:
    /**
     * @brief   Constructor.
     * @param   parent  (Optional) If non-null, the parent.
     */
    explicit ThemeSelector(QWidget *parent = nullptr);
    /**
     * @brief   Refreshes the dialog.
     */
    void refresh();
    /**
     * @brief   Gets selected theme directory
     * @return  The selected theme directory or @c nullptr if none selected.
     */
    const QDir *selectedThemeDir();
protected slots:
    /**
     * @brief   Slot emitted when a theme was selected.
     */
    void themeSelected();
public slots:
    void resizeEvent(QResizeEvent *event) override;
private:
    /**
     * @brief   Updates the preview image or removes it if none set.
     */
    void updatePreview();
};

// ============================================================================================= //

#endif
