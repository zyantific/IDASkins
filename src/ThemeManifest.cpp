/**
 * This file is part of IDASkins IDA PRO plugin (zyantific.com).
 * 
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Joel Höner (athre0z)
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

#include "ThemeManifest.hpp"

#include <QXmlStreamReader>

// ============================================================================================= //
// [ThemeManifest]                                                                               //
// ============================================================================================= //

ThemeManifest::ThemeManifest(const QString &manifestFilePath)
{
    parseFile(manifestFilePath);
}

void ThemeManifest::parseFile(const QString &manifestFilePath)
{
    QFile manifestFile(manifestFilePath);
    if (!manifestFile.open(QFile::ReadOnly | QFile::Text))
        throw XInvalidManifest("unable to open manifest file");
    
    QXmlStreamReader xml(&manifestFile);
    while (!xml.atEnd() && !xml.hasError())
    {
        auto token = xml.readNext();
        if (token == QXmlStreamReader::StartDocument) continue;

        if (xml.name() == "theme" && token == QXmlStreamReader::StartElement)
        {
            QXmlStreamAttributes attrs = xml.attributes();
            if (!attrs.hasAttribute("name") 
                || !attrs.hasAttribute("author") 
                || !attrs.hasAttribute("version"))
            {
                throw XInvalidManifest("manifest lacks essential tags");
            }

            m_themeName     = attrs.value("name").toString();
            m_themeAuthor   = attrs.value("author").toString();
            m_themeVersion  = attrs.value("version").toString();

            if (attrs.hasAttribute("preview_image"))
                m_themePreviewImage = attrs.value("preview_image").toString();
            else
                m_themePreviewImage.clear();

            if (attrs.hasAttribute("notes"))
                m_themeNotes = attrs.value("notes").toString();
            else
                m_themeNotes.clear();
        }
    }

    if (xml.hasError())
    {
        throw XInvalidManifest(
            ("error while parsing XML: " + xml.errorString()).toAscii().data());
    }
}

// ============================================================================================= //
