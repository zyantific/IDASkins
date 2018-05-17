/**
 * This file is part of IDASkins IDA PRO plugin (zyantific.com).
 * 
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Joel H�ner (athre0z)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish, 
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "ThemeSelector.hpp"

#include "Utils.hpp"
#include "ObjectInspector.hpp"
#include "PluginConfig.hpp"

#include <memory>
#include <QPixmap>
#include <idp.hpp>
#include "Core.hpp"

// ============================================================================================= //
// [ThemeSelector]                                                                               //
// ============================================================================================= //

ThemeSelector::ThemeSelector(QWidget *parent)
    : QDialog(parent)
{
    setModal(false);
    m_widgets.setupUi(this);
    connect(m_widgets.lwSkinSelection, SIGNAL(itemSelectionChanged()), SLOT(themeSelected()));
    connect(m_widgets.btnObjectInspector, SIGNAL(clicked()), 
        &Core::instance(), SLOT(openObjectInspector()));
    connect(m_widgets.leClrPathVal, SIGNAL(selectionChanged()), this, SLOT(clrPathSelectionChanged()));
    refresh();
}

void ThemeSelector::refresh()
{
    // Build list of themes
    auto themesDir = Utils::getThemesDir();
    auto themeDirs = themesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    auto themes = std::unique_ptr<ThemeList>(new ThemeList);
    for (auto it = themeDirs.begin(); it != themeDirs.end(); ++it)
    {
        try
        {
            QDir curThemeDir = themesDir.absolutePath() + "/" + *it;
            themes->emplace_back(std::make_pair(
                curThemeDir, std::move(std::unique_ptr<ThemeManifest>(
                new ThemeManifest(curThemeDir.absolutePath() + "/manifest.xml")))));
        }
        catch (const ThemeManifest::XInvalidManifest &e) 
        {
            msg("[" PLUGIN_NAME "] %s: %s\n", it->toUtf8().data(), e.what());
        }
    }

    // Publish theme list
    m_widgets.lwSkinSelection->clear();
    for (auto it = themes->begin(); it != themes->end(); ++it)
    {
        m_widgets.lwSkinSelection->addItem(it->second->themeName());
    }

    m_widgets.lwSkinSelection->setCurrentIndex(m_widgets.lwSkinSelection->rootIndex());
    m_curThemeList = std::move(themes);
}

void ThemeSelector::clrPathSelectionChanged()
{
    if(!settingSelection)
    {
        settingSelection = true;
        m_widgets.leClrPathVal->selectAll();
        settingSelection = false;
    }
}

void ThemeSelector::themeSelected()
{
    Q_ASSERT(m_curThemeList);
    Q_ASSERT(m_widgets.lwSkinSelection->selectionModel());
    auto selectedIndicies = m_widgets.lwSkinSelection->selectionModel()->selectedIndexes();
    Q_ASSERT(selectedIndicies.count() == 1);
    Q_ASSERT(static_cast<std::size_t>(selectedIndicies.first().row()) < m_curThemeList->size());
    const auto &entry = m_curThemeList->at(selectedIndicies.first().row());

    m_widgets.lblAuthorVal->setText(entry.second->themeAuthor());
    m_widgets.lblVersionVal->setText(entry.second->themeVersion());
    m_widgets.lblNotesVal->setText(entry.second->themeNotes());

    // Find first CLR file in theme dir
    auto clrFiles = entry.first.entryList(QStringList()<<"*.clr", QDir::Files);
    m_widgets.leClrPathVal->setText(clrFiles.begin() != clrFiles.end() ? QDir::toNativeSeparators(entry.first.absolutePath() + "/" + *clrFiles.begin()) : "");
    m_widgets.leClrPathVal->selectAll();

    if (!entry.second->themePreviewImage().isEmpty())
    {
        m_curPreviewImage 
            = QPixmap(entry.first.absolutePath() + "/" + entry.second->themePreviewImage());
    }
    else
    {
        m_curPreviewImage = QPixmap();
    }

    updatePreview();
}

void ThemeSelector::updatePreview()
{
    auto lblPreview = m_widgets.lblPreview;
    if (!m_curPreviewImage)
    {
        lblPreview->setText("no preview available");
    }
    else
    {
        lblPreview->setPixmap(m_curPreviewImage.scaled(
            lblPreview->width(), lblPreview->height(), 
            Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void ThemeSelector::resizeEvent(QResizeEvent *event)
{
    this->QDialog::resizeEvent(event);
    updatePreview();
}

const QDir* ThemeSelector::selectedThemeDir()
{
    Q_ASSERT(m_curThemeList);
    Q_ASSERT(m_widgets.lwSkinSelection->selectionModel());
    auto selectedIndicies = m_widgets.lwSkinSelection->selectionModel()->selectedIndexes();

    return selectedIndicies.count() == 0 ? nullptr 
        : &m_curThemeList->at(selectedIndicies.first().row()).first;
}

// ============================================================================================= //
