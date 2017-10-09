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

#include <QFile>

#ifndef THEMEMANIFEST_HPP
#define THEMEMANIFEST_HPP

// ============================================================================================= //
// [ThemeManifest]                                                                               //
// ============================================================================================= //

class ThemeManifest
{
    Q_DISABLE_COPY(ThemeManifest)

    QString m_themeName;
    QString m_themeAuthor;
    QString m_themeVersion;
    QString m_themePreviewImage;
    QString m_themeNotes;
public:
    ThemeManifest() {}
    ThemeManifest(const QString &manifestFilePath);
public:
    struct XInvalidManifest : std::runtime_error
    {
        XInvalidManifest(const char *what) : std::runtime_error(what) {}
    };
public:
    const QString& themeName        () const { return m_themeName;           }
    const QString& themeAuthor      () const { return m_themeAuthor;         }
    const QString& themeVersion     () const { return m_themeVersion;        }
    const QString& themePreviewImage() const { return m_themePreviewImage;   }
    const QString& themeNotes       () const { return m_themeNotes;          }
public:
    /**
     * @brief   Parses a given manifest file.
     * @param   manifestFilePath    Full pathname of the manifest file.
     * @throws  XInvalidManifest    If manifest file is invalid.
     */
    void parseFile(const QString &manifestFilePath);
};

// ============================================================================================= //

#endif
